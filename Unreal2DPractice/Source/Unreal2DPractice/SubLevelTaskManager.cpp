#include "SubLevelTaskManager.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UISelectedManager.h"

USubLevelTaskManager* USubLevelTaskManager::Instance = nullptr;

USubLevelTaskManager* USubLevelTaskManager::Get(UWorld* World)
{
    if (!Instance)
    {
        if (!World)
        {
            UE_LOG(LogTemp, Error, TEXT("Get called with null World"));
            return nullptr;
        }

        UObject* Outer = World->GetGameInstance();
        Instance = NewObject<USubLevelTaskManager>(Outer);
        Instance->SetFlags(RF_Transient);
        Instance->WorldContext = World;
    }
    else
    {
        Instance->WorldContext = World;
    }

    return Instance;
}

void USubLevelTaskManager::RegisterWidget(UUserWidget* Widget)
{
    if (!Widget)
        return;

    if (!Widget->GetClass()->ImplementsInterface(UTaskWidgetInterface::StaticClass()))
        return;

    RegisteredWidgets.Add(Widget);

    bool bRunning = (PendingTasks.Num() > 0 || ActiveMoveTasks.Num() > 0);
    ITaskWidgetInterface::Execute_UpdateTaskState(Widget, bRunning);
}

void USubLevelTaskManager::NotifyWidgets(bool bRunning)
{
    for (auto& W : RegisteredWidgets)
    {
        if (W.GetObject() && W.GetInterface())
        {
            W->Execute_UpdateTaskState(W.GetObject(), bRunning);
        }
    }
}

void USubLevelTaskManager::RequestTask(UDelayedTaskData* TaskData)
{
    if (!TaskData) return;

    PendingTasks.Add(TaskData);

    UE_LOG(LogTemp, Log, TEXT("Task Requested!"));

    NotifyWidgets(true);
}

void USubLevelTaskManager::OnSubLevelEntered()
{
    for (auto* Data : PendingTasks)
    {
        ScheduleTask(Data);
    }

    PendingTasks.Empty();
}

void USubLevelTaskManager::ScheduleTask(UDelayedTaskData* TaskData)
{
    if (!TaskData || !WorldContext.IsValid()) return;

    FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &USubLevelTaskManager::ExecuteTask, TaskData);

    FTimerHandle Handle;
    WorldContext->GetTimerManager().SetTimer(Handle, Delegate, TaskData->Delay, false);
    UE_LOG(LogTemp, Log, TEXT("Scheduled Task for %s (Delay: %.2f)"),
        TaskData->TargetActor.IsValid() ? *TaskData->TargetActor->GetName() : TEXT("NULL"),
        TaskData->Delay);

    UUISelectedManager* State = WorldContext->GetGameInstance()->GetSubsystem<UUISelectedManager>();
    State->LastSelectedStation = "";
}

void USubLevelTaskManager::ExecuteTask(UDelayedTaskData* TaskData)
{
    if (!TaskData) return;

    AActor* Actor = TaskData->TargetActor.Get();
    if (!Actor) return;

    if (!TaskData->StartLocation.IsZero())
    {
        Actor->SetActorLocation(TaskData->StartLocation);
    }

    FVector Dir = (TaskData->TargetTransform - Actor->GetActorLocation()).GetSafeNormal();

    FMoveTask NewTask;
    NewTask.Actor = Actor;
    NewTask.TaskData = TaskData;
    NewTask.TargetLocation = TaskData->TargetTransform;
    NewTask.MoveDirection = Dir;
    NewTask.MoveSpeed = TaskData->MoveSpeed;
    NewTask.Phase = EMovePhase::MovingToTarget;

    ActiveMoveTasks.Add(NewTask);

    if (WorldContext.IsValid() && !WorldContext->GetTimerManager().IsTimerActive(MoveTimerHandle))
    {
        WorldContext->GetTimerManager().SetTimer(
            MoveTimerHandle,
            this,
            &USubLevelTaskManager::TickMove,
            0.01f,
            true
        );
    }

    UE_LOG(LogTemp, Log, TEXT("ExecuteTask started for %s"), *Actor->GetName());
}

void USubLevelTaskManager::TickMove()
{
    if (!WorldContext.IsValid()) return;

    constexpr float Delta = 0.01f;
    TArray<int32> Completed;

    for (int32 i = 0; i < ActiveMoveTasks.Num(); i++)
    {
        FMoveTask& Task = ActiveMoveTasks[i];

        if (!Task.Actor.IsValid())
        {
            Completed.Add(i);
            continue;
        }

        switch (Task.Phase)
        {
        case EMovePhase::MovingToTarget:
            HandleMoveToTarget(Task);
            break;

        case EMovePhase::Waiting:
            HandleWaiting(Task, Delta);
            break;

        case EMovePhase::MovingForward:
            if (HandleMoveForward(Task, Delta))
            {
                Completed.Add(i);
            }
            break;
        }
    }

    for (int32 i = Completed.Num() - 1; i >= 0; i--)
        ActiveMoveTasks.RemoveAt(Completed[i]);

    if (ActiveMoveTasks.Num() == 0)
    {
        NotifyWidgets(false);
        WorldContext->GetTimerManager().ClearTimer(MoveTimerHandle);
    }
}

void USubLevelTaskManager::HandleMoveToTarget(FMoveTask& Task)
{
    AActor* Actor = Task.Actor.Get();
    if (!Actor) return;

    FVector Cur = Actor->GetActorLocation();
    float Dist = FVector::Dist(Cur, Task.TargetLocation);

    if (Dist < 1.f)
    {
        // µµÂø Ã³¸®
        if (Task.TaskData->bIsAnswer)
        {
            //OpenSubwayDoor(Actor);
            //OpenScreenDoor(Actor);
            Task.WaitRemaining = Task.TaskData->Delay * 2.f;
        }
        else
        {
            Task.WaitRemaining = Task.TaskData->Delay;
        }

        Task.Phase = EMovePhase::Waiting;
        return;
    }

    Actor->SetActorLocation(
        Cur + Task.MoveDirection * Task.MoveSpeed * 0.01f
    );
}

void USubLevelTaskManager::HandleWaiting(FMoveTask& Task, float Delta)
{
    Task.WaitRemaining -= Delta;

    if (Task.WaitRemaining > 0.f)
        return;

    if (Task.TaskData->bIsAnswer)
    {
        //CloseSubwayDoor(Task.Actor.Get());
    }

    Task.ForwardMoveRemaining = 15.f;
    Task.Phase = EMovePhase::MovingForward;
}

bool USubLevelTaskManager::HandleMoveForward(FMoveTask& Task, float Delta)
{
    AActor* Actor = Task.Actor.Get();
    if (!Actor) return true;

    Actor->SetActorLocation(
        Actor->GetActorLocation() +
        Task.MoveDirection * Task.MoveSpeed * Delta
    );

    Task.ForwardMoveRemaining -= Delta;
    return Task.ForwardMoveRemaining <= 0.f;
}

void USubLevelTaskManager::BeginDestroy()
{
    Instance = nullptr;
    if (WorldContext.IsValid())
        WorldContext->GetTimerManager().ClearAllTimersForObject(this);

    Super::BeginDestroy();
}
