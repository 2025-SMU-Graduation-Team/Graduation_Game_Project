// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InventoryComponent.h"
#include "DoorInteractionActor.h"
#include "InteractableActor.h"
#include "MyPaperCharacter.generated.h"

/**
 * 
 */

class AHidingSpot;

UCLASS()
class UNREAL2DPRACTICE_API AMyPaperCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:
	AMyPaperCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Animation
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	// State Function
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void StartJump(const FInputActionValue& Value);

	UFUNCTION()
	void StopJump(const FInputActionValue& Value);

	UFUNCTION()
	void Interact(const FInputActionValue& Value);

	UFUNCTION()
	void PlayDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AInteractableActor* CurrentInteractable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryComponent* Inventory;

private:
	UPROPERTY(EditAnywhere, Category = "Animation")
	class UPaperFlipbook* IdleAnimation;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UPaperFlipbook* RunAnimation;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UPaperFlipbook* JumpAnimation;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UPaperFlipbook* DieAnimation;

	void UpdateAnimation();
	void UpdateCharacterDirection(float AxisValue);
};