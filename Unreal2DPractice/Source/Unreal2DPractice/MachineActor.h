// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "GameFramework/Actor.h"
#include "MachineActor.generated.h"

UCLASS()
class UNREAL2DPRACTICE_API AMachineActor : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMachineActor();

	virtual void Interact() override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Machine")
	TSubclassOf<UUserWidget> WBPMachine;

};
