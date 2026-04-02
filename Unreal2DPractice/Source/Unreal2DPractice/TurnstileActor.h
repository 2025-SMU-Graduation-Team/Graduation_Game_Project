// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.h"
#include "PlayerMessageWidget.h"
#include "TurnstileActor.generated.h"

UCLASS()
class UNREAL2DPRACTICE_API ATurnstileActor : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurnstileActor();
	virtual void Interact() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void PlayOpenSoundAndUnlock();

	UPROPERTY(EditAnywhere, Category = "Turnstile", meta = (ClampMin = "0.0"))
	float OpenSoundDelay = 0.08f;

	FTimerHandle OpenSoundTimerHandle;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UPlayerMessageWidget> PlayerMessageWidgetClass;

};
