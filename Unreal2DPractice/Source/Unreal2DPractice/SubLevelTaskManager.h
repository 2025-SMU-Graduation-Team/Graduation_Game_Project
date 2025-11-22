// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
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

	void RequestTask(FName ObjectID);
	void OnSubLevelEntered();
	void ScheduleTask(FName ObjectID, float Delay);

private:
	static USubLevelTaskManager* Instance;
	void PerformTask(FName ObjectID);
	UPROPERTY()
	TArray<FName> PendingTasks;
};
