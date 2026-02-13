// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InventoryComponent.h"
#include "DoorInteractionActor.h"
#include "InteractableActor.h"
#include "SubwayStateActor.h"
#include "MyPaperCharacter.generated.h"

/**
 * 
 */

class AHidingSpot;
class UPlayerCameraController;

UCLASS()
class UNREAL2DPRACTICE_API AMyPaperCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:
	AMyPaperCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void GoToGameOverLevel();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool bEnableMovement = true;

	// Animation
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SelectSlotAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> UseItemAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> HideAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> EnterCutSceneAction;

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
	void EnterCutScene(const FInputActionValue& Value);

	void OnSelectSlot(const FInputActionValue& Value);
	void OnUseItem(const FInputActionValue& Value);

	UFUNCTION()
	void PlayDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead = false;

	// Hide Function
	UFUNCTION()
	void EnterHide();

	UFUNCTION()
	void ExitHide();

	UFUNCTION()
	void SetCanHide(AHidingSpot* Spot);
	void ClearCanHide(AHidingSpot* Spot);

	UFUNCTION()
	void OnHidePressed(const FInputActionValue& Value);

	UFUNCTION()
	void OnHideReleased(const FInputActionValue& Value);

	void SetCurrentSubway(ASubwayStateActor* Subway);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hide")
	bool bIsHidden = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hide")
	bool bCanHide = false;

	UPROPERTY()
	AHidingSpot* CurrentHidingSpot = nullptr;

	UPROPERTY()
	FVector DefaultSpriteOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AInteractableActor* CurrentInteractable;

	UPROPERTY()
	ASubwayStateActor* CurrentSubway;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryComponent* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPlayerCameraController* CameraController;

private:
	UPROPERTY(EditAnywhere, Category = "Animation")
	class UPaperFlipbook* IdleAnimation;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UPaperFlipbook* RunAnimation;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UPaperFlipbook* JumpAnimation;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UPaperFlipbook* DieAnimation;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UPaperFlipbook* HideAnimation;

	void UpdateAnimation();
	void UpdateCharacterDirection(float AxisValue);
};