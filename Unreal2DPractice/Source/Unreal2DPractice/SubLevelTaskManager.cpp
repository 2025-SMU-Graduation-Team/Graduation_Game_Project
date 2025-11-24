// Fill out your copyright notice in the Description page of Project Settings.


#include "SubLevelTaskManager.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"

USubLevelTaskManager* USubLevelTaskManager::Instance = nullptr;

USubLevelTaskManager* USubLevelTaskManager::Get(UWorld* World)
{
	if (!Instance)
	{
		if (!World)
		{
			UE_LOG(LogTemp, Error, TEXT("SubLevelTaskManager::Get called with null World!"));
			return nullptr;
		}

		Instance = NewObject<USubLevelTaskManager>(World);
		Instance->WorldContext = World;
		UE_LOG(LogTemp, Log, TEXT("SubLevelTaskManager created for World: %s"), *World->GetName());
	}
	else if (Instance->WorldContext != World && World)
	{
		Instance->WorldContext = World;
		UE_LOG(LogTemp, Log, TEXT("SubLevelTaskManager world context updated to: %s"), *World->GetName());
	}

	return Instance;
}

void USubLevelTaskManager::RequestTask(const FDelayedTask& Task)
{
	PendingTasks.Add(Task);
	UE_LOG(LogTemp, Log, TEXT("Task Requested: %s"), *Task.TargetActor->GetActorLabel());
}

void USubLevelTaskManager::OnSubLevelEntered()
{
	UE_LOG(LogTemp, Log, TEXT("OnSubLevelEntered - scheduling %d tasks"), PendingTasks.Num());

	for (const FDelayedTask& Task : PendingTasks)
	{
		ScheduleTask(Task);
	}

	PendingTasks.Empty();
}

void USubLevelTaskManager::ScheduleTask(const FDelayedTask& Task)
{
	UWorld* World = GetWorld();
	if (!World || !Task.TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("ScheduleTask: Invalid Task or actor"));
		return;
	}


	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(
		this, &USubLevelTaskManager::PerformTask, Task);

	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, Delegate, Task.Delay, false);

	UE_LOG(LogTemp, Log, TEXT("Scheduled Task for %s (Delay: %.1f)"),
		*Task.TargetActor->GetActorLabel(), Task.Delay);
}

void USubLevelTaskManager::PerformTask(FDelayedTask Task)
{
	if (!Task.TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("PerformTask: TargetActor is null"));
		return;
	}

	// 원하는 방식 선택: Transform 그대로 이동 vs 상대 이동
	FVector FinalLoc = Task.TargetActor->GetActorLocation() + Task.TargetTransform.GetLocation();

	Task.TargetActor->SetActorLocation(FinalLoc);

	UE_LOG(LogTemp, Log, TEXT("PerformTask Executed on %s"), *Task.TargetActor->GetActorLabel());
}

