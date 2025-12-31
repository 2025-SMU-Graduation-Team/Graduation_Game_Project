#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DelayedTaskData.h"
#include "TaskWidgetInterface.h"
#include "UObject/Object.h"
#include "SubLevelTaskManager.generated.h"

class UDelayedTaskData;
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

    FVector TargetLocation;
    FVector MoveDirection;

    float MoveSpeed = 0.f;
    float WaitRemaining = 0.f;
    float ForwardMoveRemaining = 0.f;

    EMovePhase Phase = EMovePhase::MovingToTarget;
};

UCLASS()
class UNREAL2DPRACTICE_API USubLevelTaskManager : public UObject
{
    GENERATED_BODY()

    public:
    static USubLevelTaskManager* Get(UWorld* World);

    void RegisterWidget(UUserWidget* Widget);
    void RequestTask(UDelayedTaskData* TaskData);
    void OnSubLevelEntered();

protected:
    virtual void BeginDestroy() override;

private:
    void NotifyWidgets(bool bRunning);

    void ScheduleTask(UDelayedTaskData* TaskData);
    void ExecuteTask(UDelayedTaskData* TaskData);

    void TickMove();

    void HandleMoveToTarget(FMoveTask& Task);
    void HandleWaiting(FMoveTask& Task, float Delta);
    bool HandleMoveForward(FMoveTask& Task, float Delta);

    void OpenDoor(AActor* Actor);
    void CloseDoor(AActor* Actor);

private:
    static USubLevelTaskManager* Instance;

    TWeakObjectPtr<UWorld> WorldContext;

    TArray<UDelayedTaskData*> PendingTasks;
    TArray<FMoveTask> ActiveMoveTasks;

    TArray<TScriptInterface<class UTaskWidgetInterface>> RegisteredWidgets;

    FTimerHandle MoveTimerHandle;

    TWeakObjectPtr<ASubwayStateActor> SubwayStateActor;
};
