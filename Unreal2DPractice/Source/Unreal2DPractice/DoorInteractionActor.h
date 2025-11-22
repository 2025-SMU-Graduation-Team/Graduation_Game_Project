// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "DoorInteractionActor.generated.h"

class UBoxComponent;

UCLASS()
class UNREAL2DPRACTICE_API ADoorInteractionActor : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorInteractionActor();

protected:
	virtual void Interact() override;

public:
	UPROPERTY(EditAnywhere, Category = "Door")
	FName TargetLevelName;

	UPROPERTY(EditAnywhere, Category = "Door")
	FVector TargetTeleportLocation;
};
