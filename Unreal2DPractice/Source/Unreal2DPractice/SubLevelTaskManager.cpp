#include "SubLevelTaskManager.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UISelectedManager.h"
#include "OpeningDoorInterface.h"
#include "LevelChangeActor.h"
#include "SubwayStateActor.h"
#include "DelayedTaskData.h"
#include "Components/SceneComponent.h"
#include "Misc/PackageName.h"

namespace
{
template <typename SoftPtrType>
FTaskActorBinding MakeBinding(const SoftPtrType& SoftPtr)
{
    FTaskActorBinding Binding;
    const FSoftObjectPath Path = SoftPtr.ToSoftObjectPath();
    Binding.ObjectPath = Path;
    Binding.FullPath = Path.ToString();
    Binding.SubPath = Path.GetSubPathString();
    Binding.AssetPath = Path.GetAssetPathString();
    Binding.AssetName = FPackageName::GetShortName(Binding.AssetPath);

    FString LeftPart;
    FString RightPart;
    if (Binding.SubPath.Split(TEXT("."), &LeftPart, &RightPart, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
    {
        Binding.ActorName = RightPart;
    }
    else
    {
        Binding.ActorName = Binding.SubPath;
    }

    return Binding;
}
}

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

    CaptureTaskBindings(TaskData);

    if (HasLoadedSubwayStateActors())
    {
        SetReservationActiveForTask(TaskData, true);
    }

    UE_LOG(LogTemp, Warning, TEXT("RequestTask registered"));
    PendingTasks.Add(TaskData);

    if (HasLoadedSubwayStateActors())
    {
        RefreshSubwayLockStates();
    }

    NotifyWidgets(true);
}

void USubLevelTaskManager::OnSubLevelEntered()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    if (bSubLevelEnteredProcessingQueued)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] OnSubLevelEntered already queued. PendingTasks=%d"), PendingTasks.Num());
        return;
    }

    bSubLevelEnteredProcessingQueued = true;
    UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] Queue OnSubLevelEntered. PendingTasks=%d"), PendingTasks.Num());
    World->GetTimerManager().SetTimerForNextTick(this, &USubLevelTaskManager::ProcessSubLevelEntered);
}

void USubLevelTaskManager::ProcessSubLevelEntered()
{
    bSubLevelEnteredProcessingQueued = false;

    UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] ProcessSubLevelEntered. PendingTasks=%d"), PendingTasks.Num());
    CachedSubwayStateActors.Empty();
    CachedTargetActors.Empty();
    CachedDoorActors.Empty();

    for (UDelayedTaskData* Task : PendingTasks)
    {
        ScheduleTask(Task);
    }

    PendingTasks.Empty();
    RefreshSubwayLockStates();
}

void USubLevelTaskManager::SetReservationActiveForTask(UDelayedTaskData* TaskData, bool bActive)
{
    if (!IsValid(TaskData))
    {
        return;
    }

    if (ASubwayStateActor* SubwayStateActor = ResolveSubwayStateActor(TaskData))
    {
        SubwayStateActor->SetReservationActive(bActive);
    }
}

void USubLevelTaskManager::CaptureTaskBindings(const UDelayedTaskData* TaskData)
{
    if (!IsValid(TaskData))
    {
        return;
    }

    FDelayedTaskBindings& Bindings = CapturedTaskBindings.FindOrAdd(TaskData);
    Bindings.TargetActor = MakeBinding(TaskData->TargetActor);
    Bindings.ScreenDoorActor = MakeBinding(TaskData->ScreenDoorActor);
    Bindings.SubwayDoorActor = MakeBinding(TaskData->SubwayDoorActor);
    Bindings.SubwayStateActor = MakeBinding(TaskData->SubwayStateActor);
}

bool USubLevelTaskManager::HasLoadedSubwayStateActors() const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    for (TActorIterator<ASubwayStateActor> It(World); It; ++It)
    {
        if (*It)
        {
            return true;
        }
    }

    return false;
}

void USubLevelTaskManager::ScheduleTask(UDelayedTaskData* TaskData)
{
    if (!TaskData || !GetWorld())
        return;

    UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] ScheduleTask task=%p delay=%.2f"), TaskData, TaskData->Delay);

    FTimerHandle ExecuteTaskHandle;
    FTimerDelegate Delegate =
        FTimerDelegate::CreateUObject(this, &USubLevelTaskManager::ExecuteTask, TaskData);

    GetWorld()->GetTimerManager().SetTimer(
        ExecuteTaskHandle,
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

    UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] ExecuteTask task=%p"), TaskData);

    AActor* Actor = ResolveTargetActor(TaskData);
    if (!Actor)
    {
        UE_LOG(LogTemp, Error, TEXT("[SubLevelTaskManager] Failed to resolve target actor for task '%s'"), *GetNameSafe(TaskData));
        SetReservationActiveForTask(TaskData, false);
        RefreshSubwayLockStates();
        return;
    }

    if (USceneComponent* RootComponent = Actor->GetRootComponent();
        RootComponent && RootComponent->Mobility != EComponentMobility::Movable)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] Promoting target actor '%s' root mobility from %d to Movable"),
            *GetNameSafe(Actor),
            static_cast<int32>(RootComponent->Mobility));
        RootComponent->SetMobility(EComponentMobility::Movable);
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
        const FMoveTask& CompletedTask = ActiveMoveTasks[CompletedTasks[i]];
        if (IsValid(CompletedTask.TaskData))
        {
            SetReservationActiveForTask(CompletedTask.TaskData, false);
        }
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
            TArray<AActor*> DoorActors;
            ResolveDoorActors(Task.TaskData, DoorActors);
            for (AActor* DoorActor : DoorActors)
            {
                OpenDoor(DoorActor);
            }
            Task.WaitRemaining = Task.TaskData->Delay * 240.f;
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
    if (Task.TaskData && Task.TaskData->bIsAnswer)
    {
        if (ASubwayStateActor* SubwayStateActor = ResolveSubwayStateActor(Task.TaskData))
        {
            SubwayStateActor->SetState(ESubwayState::DoorsOpen);
        }
    }

    Task.WaitRemaining -= Delta;

    if (Task.WaitRemaining > 3.f)
        return;

    if (Task.TaskData->bIsAnswer)
    {
        TArray<AActor*> DoorActors;
        ResolveDoorActors(Task.TaskData, DoorActors);
        for (AActor* DoorActor : DoorActors)
        {
            CloseDoor(DoorActor);
        }
    }

    if (Task.WaitRemaining > 0.f)
        return;

    Task.ForwardMoveRemaining = 50.f;
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
        if (Task.TaskData && Task.TaskData->bIsAnswer)
        {
            if (ASubwayStateActor* SubwayStateActor = ResolveSubwayStateActor(Task.TaskData))
            {
                SubwayStateActor->SetState(ESubwayState::Passed);
            }
        }
        return true;
    }

    return false;
}

void USubLevelTaskManager::RefreshSubwayLockStates()
{
    if (bSubLevelEnteredProcessingQueued)
    {
        return;
    }

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

    if (const TWeakObjectPtr<ASubwayStateActor>* CachedSubwayStateActor = CachedSubwayStateActors.Find(TaskData))
    {
        if (CachedSubwayStateActor->IsValid())
        {
            return CachedSubwayStateActor->Get();
        }

        CachedSubwayStateActors.Remove(TaskData);
    }

    if (const FDelayedTaskBindings* Bindings = CapturedTaskBindings.Find(TaskData))
    {
        if (Bindings->SubwayStateActor.IsBound())
        {
            if (AActor* BoundActor = ResolveActorFromBinding(Bindings->SubwayStateActor))
            {
                if (ASubwayStateActor* BoundSubwayStateActor = Cast<ASubwayStateActor>(BoundActor))
                {
                    CachedSubwayStateActors.Add(TaskData, BoundSubwayStateActor);
                    return BoundSubwayStateActor;
                }
            }

            UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] Failed to resolve explicit SubwayStateActor binding for task '%s'"), *GetNameSafe(TaskData));
            return nullptr;
        }
    }

    AActor* ReferenceActor = ResolveReferenceActor(TaskData);
    if (!ReferenceActor)
    {
        return nullptr;
    }

    UWorld* World = GetWorld();
    if (!World)
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
        CachedSubwayStateActors.Add(TaskData, ClosestSubwayStateActor);
    }

    return ClosestSubwayStateActor;
}

AActor* USubLevelTaskManager::ResolveTargetActor(const UDelayedTaskData* TaskData) const
{
    if (!IsValid(TaskData))
    {
        return nullptr;
    }

    if (const TWeakObjectPtr<AActor>* CachedTargetActor = CachedTargetActors.Find(TaskData))
    {
        if (CachedTargetActor->IsValid())
        {
            return CachedTargetActor->Get();
        }

        CachedTargetActors.Remove(TaskData);
    }

    if (const FDelayedTaskBindings* Bindings = CapturedTaskBindings.Find(TaskData))
    {
        if (Bindings->TargetActor.IsBound())
        {
            if (AActor* BoundTargetActor = ResolveActorFromBinding(Bindings->TargetActor))
            {
                CachedTargetActors.Add(TaskData, BoundTargetActor);
                return BoundTargetActor;
            }

            UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] Failed to resolve explicit target binding for task '%s'"), *GetNameSafe(TaskData));
            return nullptr;
        }
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    FVector ReferenceLocation = !TaskData->StartLocation.IsZero()
        ? TaskData->StartLocation
        : TaskData->TargetTransform;

    if (ReferenceLocation.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] No valid reference location for target actor of task '%s'"), *GetNameSafe(TaskData));
        return nullptr;
    }

    AActor* BestCandidate = nullptr;
    float BestScore = TNumericLimits<float>::Max();

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Candidate = *It;
        if (!Candidate ||
            Candidate->IsA<ASubwayStateActor>() ||
            Candidate->IsA<ALevelChangeActor>() ||
            Candidate->IsA<APawn>() ||
            Candidate->GetClass()->ImplementsInterface(UOpeningDoorInterface::StaticClass()))
        {
            continue;
        }

        const float DistanceSq = FVector::DistSquared(Candidate->GetActorLocation(), ReferenceLocation);
        float Score = DistanceSq;
        const FString CandidateName = Candidate->GetName();

        if (CandidateName.Contains(TEXT("Subway"), ESearchCase::IgnoreCase) ||
            CandidateName.Contains(TEXT("Train"), ESearchCase::IgnoreCase))
        {
            Score *= 0.1f;
        }

        if (const USceneComponent* RootComponent = Candidate->GetRootComponent();
            RootComponent && RootComponent->Mobility == EComponentMobility::Movable)
        {
            Score *= 0.5f;
        }

        if (Score < BestScore)
        {
            BestScore = Score;
            BestCandidate = Candidate;
        }
    }

    if (BestCandidate)
    {
        CachedTargetActors.Add(TaskData, BestCandidate);
        UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] Resolved target actor '%s' for task '%s'"),
            *GetNameSafe(BestCandidate),
            *GetNameSafe(TaskData));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] Could not find target actor for task '%s' near %s"),
            *GetNameSafe(TaskData),
            *ReferenceLocation.ToCompactString());
    }

    return BestCandidate;
}

void USubLevelTaskManager::ResolveDoorActors(const UDelayedTaskData* TaskData, TArray<AActor*>& OutDoorActors) const
{
    OutDoorActors.Reset();

    if (!IsValid(TaskData))
    {
        return;
    }

    if (const TArray<TWeakObjectPtr<AActor>>* CachedDoors = CachedDoorActors.Find(TaskData))
    {
        for (const TWeakObjectPtr<AActor>& CachedDoor : *CachedDoors)
        {
            if (CachedDoor.IsValid())
            {
                OutDoorActors.Add(CachedDoor.Get());
            }
        }

        if (OutDoorActors.Num() > 0)
        {
            return;
        }

        CachedDoorActors.Remove(TaskData);
    }

    TArray<TWeakObjectPtr<AActor>> ResolvedDoors;

    if (const FDelayedTaskBindings* Bindings = CapturedTaskBindings.Find(TaskData))
    {
        if (AActor* SubwayDoorActor = ResolveActorFromBinding(Bindings->SubwayDoorActor))
        {
            OutDoorActors.Add(SubwayDoorActor);
            ResolvedDoors.Add(SubwayDoorActor);
        }

        if (AActor* ScreenDoorActor = ResolveActorFromBinding(Bindings->ScreenDoorActor))
        {
            if (!OutDoorActors.Contains(ScreenDoorActor))
            {
                OutDoorActors.Add(ScreenDoorActor);
                ResolvedDoors.Add(ScreenDoorActor);
            }
        }
    }

    if (OutDoorActors.Num() == 0)
    {
        if (const FDelayedTaskBindings* Bindings = CapturedTaskBindings.Find(TaskData))
        {
            if (AActor* BoundSubwayActor = ResolveActorFromBinding(Bindings->SubwayStateActor))
            {
                if (ASubwayStateActor* SubwayStateActor = Cast<ASubwayStateActor>(BoundSubwayActor))
                {
                    for (AActor* ManagedDoorActor : SubwayStateActor->GetManagedDoorActors())
                    {
                        if (!ManagedDoorActor ||
                            !ManagedDoorActor->GetClass()->ImplementsInterface(UOpeningDoorInterface::StaticClass()))
                        {
                            continue;
                        }

                        OutDoorActors.Add(ManagedDoorActor);
                        ResolvedDoors.Add(ManagedDoorActor);
                    }
                }
            }
        }
    }

    if (OutDoorActors.Num() == 0)
    {
        UWorld* World = GetWorld();
        if (!World)
        {
            return;
        }

        FVector ReferenceLocation = TaskData->TargetTransform;
        if (ReferenceLocation.IsNearlyZero())
        {
            ReferenceLocation = TaskData->StartLocation;
        }

        if (ReferenceLocation.IsNearlyZero())
        {
            if (const FDelayedTaskBindings* Bindings = CapturedTaskBindings.Find(TaskData))
            {
                if (AActor* BoundSubwayActor = ResolveActorFromBinding(Bindings->SubwayStateActor))
                {
                    ReferenceLocation = BoundSubwayActor->GetActorLocation();
                }
            }
        }

        TArray<TPair<float, AActor*>> CandidateDoors;
        for (TActorIterator<AActor> It(World); It; ++It)
        {
            AActor* Candidate = *It;
            if (!Candidate || !Candidate->GetClass()->ImplementsInterface(UOpeningDoorInterface::StaticClass()))
            {
                continue;
            }

            CandidateDoors.Emplace(FVector::DistSquared(Candidate->GetActorLocation(), ReferenceLocation), Candidate);
        }

        CandidateDoors.Sort([](const TPair<float, AActor*>& A, const TPair<float, AActor*>& B)
        {
            return A.Key < B.Key;
        });

        const int32 NumDoorsToUse = FMath::Min(2, CandidateDoors.Num());
        for (int32 Index = 0; Index < NumDoorsToUse; ++Index)
        {
            OutDoorActors.Add(CandidateDoors[Index].Value);
            ResolvedDoors.Add(CandidateDoors[Index].Value);
        }
    }

    if (ResolvedDoors.Num() > 0)
    {
        CachedDoorActors.Add(TaskData, ResolvedDoors);
    }
    else if (HasLoadedSubwayStateActors())
    {
        UE_LOG(LogTemp, Warning, TEXT("[SubLevelTaskManager] Could not resolve door actors for task '%s'"), *GetNameSafe(TaskData));
    }
}

AActor* USubLevelTaskManager::ResolveActorFromBinding(const FTaskActorBinding& Binding) const
{
    if (!Binding.IsBound())
    {
        return nullptr;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    if (!Binding.ObjectPath.IsNull())
    {
        if (UObject* ResolvedObject = Binding.ObjectPath.ResolveObject())
        {
            if (AActor* ResolvedActor = Cast<AActor>(ResolvedObject))
            {
                return ResolvedActor;
            }
        }
    }

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Candidate = *It;
        if (!Candidate)
        {
            continue;
        }

        const FString CandidatePath = Candidate->GetPathName();
        if (!Binding.FullPath.IsEmpty() && CandidatePath == Binding.FullPath)
        {
            return Candidate;
        }

        if (!Binding.ActorName.IsEmpty() && Candidate->GetName() != Binding.ActorName)
        {
            continue;
        }

        if (!Binding.AssetName.IsEmpty())
        {
            ULevel* CandidateLevel = Candidate->GetLevel();
            if (!CandidateLevel)
            {
                continue;
            }

            const FString LevelOuterName = CandidateLevel->GetOuter() ? CandidateLevel->GetOuter()->GetName() : FString();
            const FString LevelPackageName = CandidateLevel->GetOutermost() ? CandidateLevel->GetOutermost()->GetName() : FString();
            const FString LevelOuterShortName = FPackageName::GetShortName(LevelOuterName);
            const FString LevelPackageShortName = FPackageName::GetShortName(LevelPackageName);

            const bool bMatchesLevel =
                LevelOuterName.Contains(Binding.AssetName, ESearchCase::IgnoreCase) ||
                LevelPackageName.Contains(Binding.AssetName, ESearchCase::IgnoreCase) ||
                LevelOuterShortName.Equals(Binding.AssetName, ESearchCase::IgnoreCase) ||
                LevelPackageShortName.Equals(Binding.AssetName, ESearchCase::IgnoreCase);

            if (!bMatchesLevel)
            {
                continue;
            }
        }

        if (!Binding.SubPath.IsEmpty() && CandidatePath.EndsWith(Binding.SubPath))
        {
            return Candidate;
        }
    }

    return nullptr;
}

AActor* USubLevelTaskManager::ResolveReferenceActor(const UDelayedTaskData* TaskData) const
{
    if (!IsValid(TaskData))
    {
        return nullptr;
    }

    TArray<AActor*> DoorActors;
    ResolveDoorActors(TaskData, DoorActors);
    if (DoorActors.Num() > 0)
    {
        return DoorActors[0];
    }

    return ResolveTargetActor(TaskData);
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
