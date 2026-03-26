#include "SubLevelTaskManager.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UISelectedManager.h"
#include "OpeningDoorInterface.h"
#include "SubwayStateActor.h"
#include "DelayedTaskData.h"

void USubLevelTaskManager::RegisterWidget(UUserWidget* Widget)
{
    if (!Widget)
        return;

    if (!Widget->GetClass()->ImplementsInterface(UTaskWidgetInterface::StaticClass()))
        return;

    RegisteredWidgets.Add(Widget);

    const bool bRunning = PendingTasks.Num() > 0 || ActiveMoveTasks.Num() > 0;
    ITaskWidgetInterface::Execute_UpdateTaskState(Widget, bRunning);
}

void USubLevelTaskManager::NotifyWidgets(bool bRunning)
{
    for (auto& Widget : RegisteredWidgets)
    {
        UObject* Obj = Widget.GetObject();
        if (!Obj)
            continue;

        if (Widget && Widget->GetClass()->ImplementsInterface(UTaskWidgetInterface::StaticClass()))
        {
            ITaskWidgetInterface::Execute_UpdateTaskState(Obj, bRunning);
        }
    }
}

void USubLevelTaskManager::RequestTask(UDelayedTaskData* TaskData)
{
    if (!TaskData)
        return;

    if (ASubwayStateActor* SubwayStateActor = ResolveSubwayStateActor(TaskData))
    {
        SubwayStateActor->SetReservationActive(true);
    }

    UE_LOG(LogTemp, Warning, TEXT("RequestTask registered"));
    PendingTasks.Add(TaskData);
    RefreshSubwayLockStates();
    NotifyWidgets(true);
}

void USubLevelTaskManager::OnSubLevelEntered()
{
    UE_LOG(LogTemp, Warning, TEXT("OnSubLevelEntered. PendingTasks: %d"), PendingTasks.Num());

    for (UDelayedTaskData* Task : PendingTasks)
    {
        ScheduleTask(Task);
    }

    PendingTasks.Empty();
    RefreshSubwayLockStates();
}

void USubLevelTaskManager::ScheduleTask(UDelayedTaskData* TaskData)
{
    if (!TaskData || !GetWorld())
        return;

    FTimerDelegate Delegate =
        FTimerDelegate::CreateUObject(this, &USubLevelTaskManager::ExecuteTask, TaskData);

    GetWorld()->GetTimerManager().SetTimer(
        ExecuteTaskTimerHandle,
        Delegate,
        TaskData->Delay,
        false
    );

    if (UUISelectedManager* UIState =
        GetGameInstance()->GetSubsystem<UUISelectedManager>())
    {
        UIState->LastSelectedStation = NAME_None;
    }
}

void USubLevelTaskManager::ExecuteTask(UDelayedTaskData* TaskData)
{
    if (!TaskData)
    {
        RefreshSubwayLockStates();
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("ExecuteTask"));

    AActor* Actor = TaskData->TargetActor.Get();
    if (!Actor)
    {
        UE_LOG(LogTemp, Error, TEXT("TargetActor is null"));
        RefreshSubwayLockStates();
        return;
    }

    if (!TaskData->StartLocation.IsZero())
    {
        Actor->SetActorLocation(TaskData->StartLocation);
    }

    FMoveTask NewTask;
    NewTask.Actor = Actor;
    NewTask.TaskData = TaskData;
    NewTask.TargetLocation = TaskData->TargetTransform;
    NewTask.MoveDirection =
        (TaskData->TargetTransform - Actor->GetActorLocation()).GetSafeNormal();
    NewTask.MoveSpeed = TaskData->MoveSpeed;

    ActiveMoveTasks.Add(NewTask);
    RefreshSubwayLockStates();

    if (!GetWorld()->GetTimerManager().IsTimerActive(MoveTickTimerHandle))
    {
        GetWorld()->GetTimerManager().SetTimer(
            MoveTickTimerHandle,
            this,
            &USubLevelTaskManager::TickMove,
            0.01f,
            true
        );
    }
}

void USubLevelTaskManager::TickMove()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        ActiveMoveTasks.Empty();
        return;
    }

    constexpr float Delta = 0.01f;
    TArray<int32> CompletedTasks;

    for (int32 i = 0; i < ActiveMoveTasks.Num(); ++i)
    {
        FMoveTask& Task = ActiveMoveTasks[i];

        if (!Task.Actor.IsValid())
        {
            CompletedTasks.Add(i);
            continue;
        }

        if (!IsValid(Task.TaskData))
        {
            CompletedTasks.Add(i);
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
                CompletedTasks.Add(i);
            }
            break;

        default:
            CompletedTasks.Add(i);
            break;
        }
    }

    for (int32 i = CompletedTasks.Num() - 1; i >= 0; --i)
    {
        ActiveMoveTasks.RemoveAt(CompletedTasks[i]);
    }

    RefreshSubwayLockStates();

    if (ActiveMoveTasks.Num() == 0)
    {
        NotifyWidgets(false);
        World->GetTimerManager().ClearTimer(MoveTickTimerHandle);
    }
}

void USubLevelTaskManager::HandleMoveToTarget(FMoveTask& Task)
{
    AActor* Actor = Task.Actor.Get();
    if (!Actor)
        return;

    const FVector Current = Actor->GetActorLocation();
    const float Distance = FVector::Dist(Current, Task.TargetLocation);

    if (Distance < 1.f)
    {
        if (!IsValid(Task.TaskData))
        {
            Task.Phase = EMovePhase::MovingForward;
            return;
        }

        if (Task.TaskData->bIsAnswer)
        {
            OpenDoor(Task.TaskData->SubwayDoorActor.Get());
            OpenDoor(Task.TaskData->ScreenDoorActor.Get());
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
        Current + Task.MoveDirection * Task.MoveSpeed * 0.01f
    );
}

void USubLevelTaskManager::HandleWaiting(FMoveTask& Task, float Delta)
{
    if (ASubwayStateActor* SubwayStateActor = ResolveSubwayStateActor(Task.TaskData))
    {
        SubwayStateActor->SetState(ESubwayState::DoorsOpen);
    }

    Task.WaitRemaining -= Delta;

    if (Task.WaitRemaining > 3.f)
        return;

    if (Task.TaskData->bIsAnswer)
    {
        CloseDoor(Task.TaskData->SubwayDoorActor.Get());
    }

    if (Task.WaitRemaining > 0.f)
        return;

    Task.ForwardMoveRemaining = 15.f;
    Task.Phase = EMovePhase::MovingForward;
}

bool USubLevelTaskManager::HandleMoveForward(FMoveTask& Task, float Delta)
{
    AActor* Actor = Task.Actor.Get();
    if (!Actor)
        return true;

    Actor->SetActorLocation(
        Actor->GetActorLocation() +
        Task.MoveDirection * Task.MoveSpeed * Delta
    );

    if (ASubwayStateActor* SubwayStateActor = ResolveSubwayStateActor(Task.TaskData))
    {
        SubwayStateActor->SetState(ESubwayState::Leaving);
    }

    Task.ForwardMoveRemaining -= Delta;

    if (Task.ForwardMoveRemaining <= 0.f)
    {
        if (ASubwayStateActor* SubwayStateActor = ResolveSubwayStateActor(Task.TaskData))
        {
            SubwayStateActor->SetState(ESubwayState::Passed);
        }
        return true;
    }

    return false;
}

void USubLevelTaskManager::RefreshSubwayLockStates()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    TSet<TWeakObjectPtr<ASubwayStateActor>> LockedSubwayActors;

    for (UDelayedTaskData* TaskData : PendingTasks)
    {
        if (!IsValid(TaskData))
        {
            continue;
        }

        if (ASubwayStateActor* SubwayStateActor = ResolveSubwayStateActor(TaskData))
        {
            LockedSubwayActors.Add(SubwayStateActor);
        }
    }

    for (const FMoveTask& Task : ActiveMoveTasks)
    {
        if (!IsValid(Task.TaskData))
        {
            continue;
        }

        if (ASubwayStateActor* SubwayStateActor = ResolveSubwayStateActor(Task.TaskData))
        {
            LockedSubwayActors.Add(SubwayStateActor);
        }
    }

    for (TActorIterator<ASubwayStateActor> It(World); It; ++It)
    {
        ASubwayStateActor* SubwayStateActor = *It;
        if (!SubwayStateActor)
        {
            continue;
        }

        SubwayStateActor->SetLevelChangeLockActive(LockedSubwayActors.Contains(SubwayStateActor));
    }
}

ASubwayStateActor* USubLevelTaskManager::ResolveSubwayStateActor(UDelayedTaskData* TaskData) const
{
    if (!IsValid(TaskData))
    {
        return nullptr;
    }

    if (TaskData->SubwayStateActor.IsValid())
    {
        return TaskData->SubwayStateActor.Get();
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    AActor* ReferenceActor = TaskData->SubwayDoorActor.Get();
    if (!ReferenceActor)
    {
        ReferenceActor = TaskData->TargetActor.Get();
    }

    if (!ReferenceActor)
    {
        return nullptr;
    }

    ASubwayStateActor* ClosestSubwayStateActor = nullptr;
    float ClosestDistanceSq = TNumericLimits<float>::Max();
    const FVector ReferenceLocation = ReferenceActor->GetActorLocation();

    for (TActorIterator<ASubwayStateActor> It(World); It; ++It)
    {
        ASubwayStateActor* Candidate = *It;
        if (!Candidate)
        {
            continue;
        }

        const float DistanceSq = FVector::DistSquared(ReferenceLocation, Candidate->GetActorLocation());
        if (DistanceSq < ClosestDistanceSq)
        {
            ClosestDistanceSq = DistanceSq;
            ClosestSubwayStateActor = Candidate;
        }
    }

    if (ClosestSubwayStateActor)
    {
        TaskData->SubwayStateActor = ClosestSubwayStateActor;
        UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] Auto-linked SubwayStateActor '%s' for task '%s'"),
            *GetNameSafe(ClosestSubwayStateActor),
            *GetNameSafe(TaskData));
    }

    return ClosestSubwayStateActor;
}

void USubLevelTaskManager::OpenDoor(AActor* Actor)
{
    if (!Actor)
        return;

    if (Actor->GetClass()->ImplementsInterface(UOpeningDoorInterface::StaticClass()))
    {
        IOpeningDoorInterface::Execute_OpenDoor(Actor);
    }
}

void USubLevelTaskManager::CloseDoor(AActor* Actor)
{
    if (!Actor)
        return;

    if (Actor->GetClass()->ImplementsInterface(UOpeningDoorInterface::StaticClass()))
    {
        IOpeningDoorInterface::Execute_CloseDoor(Actor);
    }
}
