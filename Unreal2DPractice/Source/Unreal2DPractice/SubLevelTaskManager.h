// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DelayedTaskData.h"
#include "TaskWidgetInterface.h"
#include "SubLevelTaskManager.generated.h"

UENUM()
enum class EMovePhase : uint8
{
	MovingToTarget,
	Waiting,
	MovingForward,
};

USTRUCT()
struct FMoveTask
{
	GENERATED_BODY()

	TWeakObjectPtr<AActor> Actor;
	UDelayedTaskData* TaskData = nullptr;

	FVector StartLocation;
	FVector TargetLocation;
	FVector MoveDirection;
	float MoveSpeed = 200.f;

	float StartTime = 0.f;

	EMovePhase Phase = EMovePhase::MovingToTarget;
	float WaitRemaining = 0.f;
	float ForwardMoveRemaining = 5.f;
};

UCLASS()
class UNREAL2DPRACTICE_API USubLevelTaskManager : public UObject
{
	GENERATED_BODY()
	
	public:
	// Singleton Getter
	static USubLevelTaskManager* Get(UWorld* World);

	// Register task
	void RequestTask(UDelayedTaskData* TaskData);

	// Execute all pending tasks when sublevel loads
	void OnSubLevelEntered();

	void RegisterWidget(UUserWidget* Widget);

private:
	// Internal functions
	void ScheduleTask(UDelayedTaskData* TaskData);
	void ExecuteTask(UDelayedTaskData* TaskData);

	// Singleton instance
	static USubLevelTaskManager* Instance;

	// World context pointer (not used as owner)
	UPROPERTY()
	TWeakObjectPtr<UWorld> WorldContext;

	// Tasks waiting for sublevel load
	UPROPERTY()
	TArray<UDelayedTaskData*> PendingTasks;

	void TickMove();
	void BeginDestroy();

	TArray<FMoveTask> ActiveMoveTasks;
	FTimerHandle MoveTimerHandle;

	UPROPERTY()
	TArray<TScriptInterface<ITaskWidgetInterface>> RegisteredWidgets;

	void NotifyWidgets(bool bRunning);

	void HandleMoveToTarget(FMoveTask& Task);
	void HandleWaiting(FMoveTask& Task, float Delta);
	bool HandleMoveForward(FMoveTask& Task, float Delta);

	void OpenDoor(AActor* Actor);
	void CloseDoor(AActor* Actor);
};
