#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DelayedTaskData.h"
#include "TaskWidgetInterface.h"
#include "SubLevelTaskManager.generated.h"

class UUserWidget;
class ASubwayStateActor;

UENUM()
enum class EMovePhase : uint8
{
    MovingToTarget,
    Waiting,
    MovingForward
};

USTRUCT()
struct FMoveTask
{
    GENERATED_BODY()

    TWeakObjectPtr<AActor> Actor;
    UPROPERTY()
    TObjectPtr<UDelayedTaskData> TaskData = nullptr;

    FVector TargetLocation = FVector::ZeroVector;
    FVector MoveDirection = FVector::ZeroVector;

    float MoveSpeed = 0.f;
    float WaitRemaining = 0.f;
    float ForwardMoveRemaining = 0.f;

    EMovePhase Phase = EMovePhase::MovingToTarget;
};

struct FTaskActorBinding
{
    FSoftObjectPath ObjectPath;
    FString FullPath;
    FString SubPath;
    FString AssetPath;
    FString AssetName;
    FString ActorName;

    bool IsBound() const
    {
        return !ObjectPath.IsNull() || !FullPath.IsEmpty() || !SubPath.IsEmpty();
    }
};

struct FDelayedTaskBindings
{
    FTaskActorBinding TargetActor;
    FTaskActorBinding ScreenDoorActor;
    FTaskActorBinding SubwayDoorActor;
    FTaskActorBinding SubwayStateActor;
};

UCLASS()
class UNREAL2DPRACTICE_API USubLevelTaskManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    void RegisterWidget(UUserWidget* Widget);
    void RequestTask(UDelayedTaskData* TaskData);
    void OnSubLevelEntered();
    void RefreshSubwayLockStates();

    void CloseDoor(AActor* Actor);

private:
    void CaptureTaskBindings(const UDelayedTaskData* TaskData);
    bool HasLoadedSubwayStateActors() const;
    void ProcessSubLevelEntered();
    void SetReservationActiveForTask(UDelayedTaskData* TaskData, bool bActive);
    class ASubwayStateActor* ResolveSubwayStateActor(UDelayedTaskData* TaskData) const;
    AActor* ResolveTargetActor(const UDelayedTaskData* TaskData) const;
    void ResolveDoorActors(const UDelayedTaskData* TaskData, TArray<AActor*>& OutDoorActors) const;
    AActor* ResolveActorFromBinding(const FTaskActorBinding& Binding) const;
    AActor* ResolveReferenceActor(const UDelayedTaskData* TaskData) const;
    void NotifyWidgets(bool bRunning);

    void ScheduleTask(UDelayedTaskData* TaskData);
    void ExecuteTask(UDelayedTaskData* TaskData);

    void TickMove();

    void HandleMoveToTarget(FMoveTask& Task);
    void HandleWaiting(FMoveTask& Task, float Delta);
    bool HandleMoveForward(FMoveTask& Task, float Delta);

    void OpenDoor(AActor* Actor);

private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<UDelayedTaskData>> PendingTasks;

    UPROPERTY(Transient)
    TArray<FMoveTask> ActiveMoveTasks;

    TMap<const UDelayedTaskData*, FDelayedTaskBindings> CapturedTaskBindings;
    mutable TMap<const UDelayedTaskData*, TWeakObjectPtr<ASubwayStateActor>> CachedSubwayStateActors;
    mutable TMap<const UDelayedTaskData*, TWeakObjectPtr<AActor>> CachedTargetActors;
    mutable TMap<const UDelayedTaskData*, TArray<TWeakObjectPtr<AActor>>> CachedDoorActors;

    TArray<TScriptInterface<UTaskWidgetInterface>> RegisteredWidgets;

    FTimerHandle MoveTickTimerHandle;
    bool bSubLevelEnteredProcessingQueued = false;
};
