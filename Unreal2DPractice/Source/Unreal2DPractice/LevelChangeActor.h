// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelChangeActor.generated.h"

class AMyPaperCharacter;

UCLASS()
class UNREAL2DPRACTICE_API ALevelChangeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelChangeActor();

	UPROPERTY(EditAnywhere, Category = "Level")
	FName NextLevelName;

	UPROPERTY(EditAnywhere, Category = "Level")
	FVector TargetTeleportLocation;

protected:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY()
	AMyPaperCharacter* CachedPlayer;
};
