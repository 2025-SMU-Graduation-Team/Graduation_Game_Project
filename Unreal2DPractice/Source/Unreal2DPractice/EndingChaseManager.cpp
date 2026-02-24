// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingChaseManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEndingChaseManager::AEndingChaseManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AEndingChaseManager::BeginPlay()
{
	Super::BeginPlay();
	FWorldDelegates::LevelAddedToWorld.AddUObject(
		this,
		&AEndingChaseManager::OnLevelLoaded
	);
}

void AEndingChaseManager::NotifyReachedEnd(AEndingMonster* Monster)
{
	if (Monster != ActiveMonster) return;

	if (ActiveMonster)
	{
		ActiveMonster->Destroy();
		ActiveMonster = nullptr;
	}

	CurrentStageIndex++;

	if (Route.IsValidIndex(CurrentStageIndex))
	{
		SpawnCurrentStage();
	}
}

void AEndingChaseManager::SpawnCurrentStage()
{
	if (!Route.IsValidIndex(CurrentStageIndex)) return;
	if (!MonsterClass) return;

	const FChaseStage& Stage = Route[CurrentStageIndex];

	if (UGameplayStatics::GetCurrentLevelName(this) != Stage.LevelName.ToString())
		return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ActiveMonster = GetWorld()->SpawnActor<AEndingMonster>(
		MonsterClass, Stage.SpawnLocation, FRotator::ZeroRotator, Params);

	if (ActiveMonster)
	{
		ActiveMonster->SetMoveDirection(Stage.MoveDirection);

		ActiveMonster->SetEndLocation(Stage.EndLocation);

		ActiveMonster->SetManager(this);
	}
}

void AEndingChaseManager::StartChase()
{
	UE_LOG(LogTemp, Log, TEXT("StartChase"));

	CurrentStageIndex = 0;
	SpawnCurrentStage();
}

void AEndingChaseManager::OnLevelLoaded(ULevel* InLevel, UWorld* InWorld)
{
	if (!Route.IsValidIndex(0)) return;
	if (bFirstSpawnTriggered) return;

	UE_LOG(LogTemp, Log, TEXT("OnLevelLoaded"));

	FString LoadedLevelName = InLevel->GetOuter()->GetName();

	if (LoadedLevelName.Contains(Route[0].LevelName.ToString()))
	{
		bFirstSpawnTriggered = true;

		GetWorld()->GetTimerManager().SetTimer(
			FirstSpawnTimerHandle,
			this,
			&AEndingChaseManager::StartChase,
			FirstSpawnDelay,
			false
		);
	}
}
