// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndingMonster.h"
#include "EndingChaseManager.generated.h"

USTRUCT(BlueprintType)
struct FChaseStage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName LevelName;

	UPROPERTY(EditAnywhere)
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere)
	FVector EndLocation;

	UPROPERTY(EditAnywhere)
	FVector TurnLocation;

	UPROPERTY(EditAnywhere)
	float MoveDirection = 1.f;
};

UCLASS()
class UNREAL2DPRACTICE_API AEndingChaseManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AEndingChaseManager();

	void NotifyReachedEnd(AEndingMonster* Monster);

protected:
	virtual void BeginPlay() override;

private:
	bool TryStartChaseForLevelName(const FString& LoadedLevelName);
	void SpawnCurrentStage();
	void StartChase();

	void OnLevelLoaded(ULevel* InLevel, UWorld* InWorld);

private:
	UPROPERTY(EditAnywhere, Category = "Chase")
	TArray<FChaseStage> Route;

	UPROPERTY(EditAnywhere, Category = "Chase")
	TSubclassOf<AEndingMonster> MonsterClass;

	UPROPERTY(EditAnywhere, Category = "Chase")
	float FirstSpawnDelay = 5.f;

	FTimerHandle FirstSpawnTimerHandle;

	int32 CurrentStageIndex = 0;
	bool bHasStartedCurrentStage = false;

	UPROPERTY()
	AEndingMonster* ActiveMonster = nullptr;
};
