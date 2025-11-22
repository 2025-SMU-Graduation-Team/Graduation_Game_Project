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
		Instance = NewObject<USubLevelTaskManager>();
		Instance->AddToRoot();
	}
	return Instance;
}

void USubLevelTaskManager::RequestTask(FName ObjectID)
{
	PendingTasks.Add(ObjectID);
	UE_LOG(LogTemp, Log, TEXT("Task Requested: %s"), *ObjectID.ToString());
}

void USubLevelTaskManager::OnSubLevelEntered()
{
	for (FName Task : PendingTasks)
	{
		ScheduleTask(Task, 2.0f);
	}

	PendingTasks.Empty();
}

void USubLevelTaskManager::ScheduleTask(FName ObjectID, float Delay)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(this))
	{
		FTimerHandle Handle;

		World->GetTimerManager().SetTimer(
			Handle,
			FTimerDelegate::CreateUObject(this, &USubLevelTaskManager::PerformTask, ObjectID),
			Delay,
			false
		);

		UE_LOG(LogTemp, Log, TEXT("Scheduled Task: %s (Delay: %.1f)"), *ObjectID.ToString(), Delay);
	}
}

void USubLevelTaskManager::PerformTask(FName ObjectID)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(this))
	{
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;

			if (Actor->GetFName() == ObjectID)
			{
				UE_LOG(LogTemp, Log, TEXT("PerformTask Executing on %s"), *ObjectID.ToString());

				FVector NewLoc = Actor->GetActorLocation() + FVector(200, 0, 0);
				Actor->SetActorLocation(NewLoc);

				return;
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("PerformTask: Actor %s not found"), *ObjectID.ToString());
	}
}

