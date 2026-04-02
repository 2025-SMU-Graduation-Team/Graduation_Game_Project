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
class UPaperFlipbook;
class UAudioComponent;

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

	void SetForcedFlipbook(UPaperFlipbook* NewFlipbook);
	void ClearForcedFlipbook();
	void RefreshAfterLevelTransition();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
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

	virtual bool CanJumpInternal_Implementation() const override;

	UFUNCTION()
	void StartJump(const FInputActionValue& Value);

	UFUNCTION()
	void StopJump(const FInputActionValue& Value);

	UFUNCTION()
	void Interact(const FInputActionValue& Value);

	UFUNCTION()
	void EnterCutScene(const FInputActionValue& Value);

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

	//Inventory

	void OnSelectSlot(const FInputActionValue& Value);
	void OnUseItem(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryComponent* Inventory;

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPlayerCameraController* CameraController;

private:
	bool ResolveInitialCameraLimit();

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	TObjectPtr<UAudioComponent> WalkAudioComponent;

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

	UPROPERTY()
	class UPaperFlipbook* ForcedFlipbook = nullptr;

	void UpdateAnimation();
	void UpdateWalkAudio();
	void StartWalkLoop();
	void StopWalkLoop();
	void UpdateCharacterDirection(float AxisValue);
	bool TryInteractFromEnterKey();

	bool bPendingInitialCameraLimitResolve = true;
};
