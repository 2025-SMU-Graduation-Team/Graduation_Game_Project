// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DelayedTask.h"
#include "SubLevelTaskManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UNREAL2DPRACTICE_API USubLevelTaskManager : public UObject
{
	GENERATED_BODY()
	
public:
	static USubLevelTaskManager* Get(UWorld* World);

	void RequestTask(const FDelayedTask& Task);
	void OnSubLevelEntered();
	void ScheduleTask(const FDelayedTask& Task);
	void PerformTask(FDelayedTask Task);

	virtual UWorld* GetWorld() const override { return WorldContext; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDelayedTask> PendingTasks;

private:
	static USubLevelTaskManager* Instance;

	UPROPERTY()
	UWorld* WorldContext = nullptr;

	UPROPERTY()
	TMap<FName, FTimerHandle> ActiveTimerHandles;
};
