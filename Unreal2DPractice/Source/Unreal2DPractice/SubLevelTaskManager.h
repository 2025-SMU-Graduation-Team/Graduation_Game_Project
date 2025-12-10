// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DelayedTaskData.h"
#include "TaskWidgetInterface.h"
#include "SubLevelTaskManager.generated.h"

USTRUCT()
struct FMoveTask
{
	GENERATED_BODY()

	AActor* Actor = nullptr;
	UDelayedTaskData* TaskData = nullptr;

	FVector StartLocation;
	FVector TargetLocation;
	float MoveSpeed = 100.f;

	float StartTime = 0.f;
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
};
