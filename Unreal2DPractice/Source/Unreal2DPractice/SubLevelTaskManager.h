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
    UDelayedTaskData* TaskData = nullptr;

    FVector TargetLocation = FVector::ZeroVector;
    FVector MoveDirection = FVector::ZeroVector;

    float MoveSpeed = 0.f;
    float WaitRemaining = 0.f;
    float ForwardMoveRemaining = 0.f;

    EMovePhase Phase = EMovePhase::MovingToTarget;
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
    class ASubwayStateActor* ResolveSubwayStateActor(UDelayedTaskData* TaskData) const;
    class ASubwayStateActor* ResolveAssignedSubwayStateActor(const UDelayedTaskData* TaskData) const;
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
    TArray<UDelayedTaskData*> PendingTasks;
    TArray<FMoveTask> ActiveMoveTasks;
    mutable TMap<const UDelayedTaskData*, TWeakObjectPtr<ASubwayStateActor>> CachedSubwayStateActors;

    TArray<TScriptInterface<UTaskWidgetInterface>> RegisteredWidgets;

    FTimerHandle MoveTickTimerHandle;
};
