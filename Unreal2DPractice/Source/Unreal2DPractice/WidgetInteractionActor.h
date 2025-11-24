// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "WidgetInteractionActor.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API AWidgetInteractionActor : public AInteractableActor
{
	GENERATED_BODY()
	
public:
	AWidgetInteractionActor();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> WidgetClass;

protected:
	virtual void Interact() override;
};
