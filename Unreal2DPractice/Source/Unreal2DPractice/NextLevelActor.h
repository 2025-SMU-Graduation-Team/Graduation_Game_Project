// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "NextLevelActor.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API ANextLevelActor : public AInteractableActor
{
	GENERATED_BODY()
	
public:
	virtual void Interact() override;

	UFUNCTION(BlueprintCallable, Category = "Stage")
	void GoToNextLevel();

protected:
	UPROPERTY(EditAnywhere, Category = "Stage")
	FName NextLevelName;

	UPROPERTY(EditAnywhere, Category = "Stage")
	class ULevelSequence* CutScene;

protected:
	void PlayCutScene();

	UFUNCTION()
	void MoveToNextLevel();
};
