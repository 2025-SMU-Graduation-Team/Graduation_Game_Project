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

    FMoveTask NewTask;
    NewTask.Actor = Actor;
    NewTask.TaskData = TaskData;
    NewTask.StartLocation = Actor->GetActorLocation();
    NewTask.TargetLocation = TaskData->TargetTransform.GetLocation();
    NewTask.MoveSpeed = TaskData->MoveSpeed;

    ActiveMoveTasks.Add(NewTask);

    if (WorldContext.IsValid() && !WorldContext->GetTimerManager().IsTimerActive(MoveTimerHandle))
    {
        WorldContext->GetTimerManager().SetTimer(
            MoveTimerHandle,
            FTimerDelegate::CreateUObject(this, &USubLevelTaskManager::TickMove),
            0.01f,
            true
        );
    }

    UE_LOG(LogTemp, Log, TEXT("ExecuteTask started for %s"), *Actor->GetName());
}

void USubLevelTaskManager::TickMove()
{
    if (!WorldContext.IsValid()) return;

    TArray<int32> Completed;

    for (int32 i = 0; i < ActiveMoveTasks.Num(); i++)
    {
        FMoveTask& M = ActiveMoveTasks[i];

        if (!M.Actor)
        {
            Completed.Add(i);
            continue;
        }

        FVector Cur = M.Actor->GetActorLocation();
        FVector Target = M.TargetLocation;
        float Dist = FVector::Distance(Cur, Target);

        if (Dist < 1.f)
        {
            Completed.Add(i);
            continue;
        }

        FVector Dir = (Target - Cur).GetSafeNormal();
        FVector NewPos = Cur + Dir * M.MoveSpeed * 0.01f;
        M.Actor->SetActorLocation(NewPos);
    }

    for (int32 j = Completed.Num() - 1; j >= 0; j--) 
    {
        ActiveMoveTasks.RemoveAt(Completed[j]);
    }

    if (ActiveMoveTasks.Num() == 0)
    {
        NotifyWidgets(false);
        WorldContext->GetTimerManager().ClearTimer(MoveTimerHandle);
    }
}

void USubLevelTaskManager::BeginDestroy()
{
    Instance = nullptr;
    if (WorldContext.IsValid())
        WorldContext->GetTimerManager().ClearAllTimersForObject(this);

    Super::BeginDestroy();
}
