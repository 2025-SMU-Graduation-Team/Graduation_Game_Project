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

		UObject* Outer = World->GetGameInstance();
		if (!Outer)
		{
			UE_LOG(LogTemp, Error, TEXT("SubLevelTaskManager::Get - World has no GameInstance!"));
			return nullptr;
		}

		Instance = NewObject<USubLevelTaskManager>(Outer);
		Instance->WorldContext = World;
		Instance->SetFlags(RF_Transient);

		UE_LOG(LogTemp, Log, TEXT("SubLevelTaskManager created (Outer = GameInstance)"));
	}
	else
	{
		if (World)
		{
			Instance->WorldContext = World;
		}
	}

	return Instance;
}

void USubLevelTaskManager::RequestTask(UDelayedTaskData* TaskData)
{
	if (!TaskData)
	{
		UE_LOG(LogTemp, Warning, TEXT("RequestTask: TaskData is NULL!"));
		return;
	}

	PendingTasks.Add(TaskData);

	if (TaskData->TargetActor.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Task Requested for actor: %s"),
			*TaskData->TargetActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Task Requested with invalid TargetActor"));
	}
}

void USubLevelTaskManager::OnSubLevelEntered()
{
	UE_LOG(LogTemp, Log, TEXT("OnSubLevelEntered - scheduling %d tasks"), PendingTasks.Num());

	for (UDelayedTaskData* Data : PendingTasks)
	{
		ScheduleTask(Data);
	}

	PendingTasks.Empty();
}

void USubLevelTaskManager::ScheduleTask(UDelayedTaskData* TaskData)
{
	if (!TaskData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ScheduleTask: Invalid TaskData"));
		return;
	}

	UWorld* World = WorldContext.IsValid() ? WorldContext.Get() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ScheduleTask: No valid world"));
		return;
	}

	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(
		this,
		&USubLevelTaskManager::ExecuteTask,
		TaskData
	);

	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(
		Handle,
		Delegate,
		TaskData->Delay,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("Scheduled Task for %s (Delay: %.2f)"),
		TaskData->TargetActor.IsValid() ? *TaskData->TargetActor->GetName() : TEXT("NULL"),
		TaskData->Delay);
}

void USubLevelTaskManager::ExecuteTask(UDelayedTaskData* TaskData)
{
	if (!TaskData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteTask: TaskData is null"));
		return;
	}

	AActor* Actor = TaskData->TargetActor.Get();
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteTask: TargetActor is null! Task skipped."));
		return;
	}

	// 시작 위치 세팅
	if (!TaskData->StartLocation.IsZero())
	{
		Actor->SetActorLocation(TaskData->StartLocation);
	}

	// 이동 태스크 등록
	FMoveTask NewTask;
	NewTask.Actor = Actor;
	NewTask.TaskData = TaskData;
	NewTask.StartTime = WorldContext.IsValid() ? WorldContext->GetTimeSeconds() : 0.f;
	NewTask.StartLocation = Actor->GetActorLocation();
	NewTask.TargetLocation = TaskData->TargetTransform.GetLocation();
	NewTask.MoveSpeed = TaskData->MoveSpeed;

	ActiveMoveTasks.Add(NewTask);

	// 타이머 시작 (매 0.01초마다 TickMove)
	if (WorldContext.IsValid() && !WorldContext->GetTimerManager().IsTimerActive(MoveTimerHandle))
	{
		WorldContext->GetTimerManager().SetTimer(
			MoveTimerHandle,
			FTimerDelegate::CreateUObject(this, &USubLevelTaskManager::TickMove),
			0.01f,
			true
		);
	}

	UE_LOG(LogTemp, Log, TEXT("ExecuteTask started for %s"), *Actor->GetName());
}

void USubLevelTaskManager::TickMove()
{
	if (!WorldContext.IsValid())
		return;

	TArray<int32> Completed;

	for (int32 i = 0; i < ActiveMoveTasks.Num(); i++)
	{
		FMoveTask& M = ActiveMoveTasks[i];

		if (!M.Actor)
		{
			Completed.Add(i);
			continue;
		}

		FVector Cur = M.Actor->GetActorLocation();
		FVector Target = M.TargetLocation;

		float Dist = FVector::Distance(Cur, Target);
		if (Dist < 1.f)
		{
			Completed.Add(i);
			continue;
		}

		FVector Dir = (Target - Cur).GetSafeNormal();
		FVector NewPos = Cur + Dir * M.MoveSpeed * 0.01f;

		M.Actor->SetActorLocation(NewPos);
	}

	for (int32 Index = Completed.Num() - 1; Index >= 0; Index--)
	{
		ActiveMoveTasks.RemoveAt(Completed[Index]);
	}

	if (ActiveMoveTasks.Num() == 0)
	{
		WorldContext->GetTimerManager().ClearTimer(MoveTimerHandle);
	}
}

void USubLevelTaskManager::BeginDestroy()
{
	if (WorldContext.IsValid())
	{
		WorldContext->GetTimerManager().ClearAllTimersForObject(this);
	}

	Super::BeginDestroy();
}
