// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EItemType.h"
#include "ItemActor.h"
#include "ItemInventoryWidget.h"

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
	void PlayDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead = false;

	// Hide Function
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hide")
	bool bIsHidden = false;        // 현재 숨은 상태인가?

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hide")
	bool bCanHide = false;         // 근처에 숨을 수 있는 오브젝트가 있는가?

	UPROPERTY()
	AHidingSpot* CurrentHidingSpot = nullptr;   // 지금 숨을 수 있는 대상

	// Inventory
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UItemInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	UItemInventoryWidget* InventoryWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TArray<UTexture2D*> InventoryItems;

	void RequestItemPickup(class AItemActor* Item);

	UFUNCTION(BlueprintCallable)
	void ConfirmPickupYes();

	UFUNCTION(BlueprintCallable)
	void ConfirmPickupNo();

	UFUNCTION()
	void InteractE(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AInteractableActor* CurrentInteractable;

	UFUNCTION()
	void EnterHide();
	void ExitHide();
	void SetCanHide(AHidingSpot* Spot); // HidingSpot에서 호출 (Overlap)
	void ClearCanHide(AHidingSpot* Spot);

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

	UPROPERTY()
	AActor* CurrentInteractable;

	void Interact();
	void AddItem(UTexture2D* Item);
	void UpdateInventoryUI();

	UPROPERTY()
	class AItemActor* PendingItem;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};