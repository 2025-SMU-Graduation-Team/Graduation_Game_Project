// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

// Forward Declaration
class UTexture2D;
class UItemInventoryWidget;
class AItemActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREAL2DPRACTICE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	/** Item List */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UTexture2D*> Items;

	/** UI Widget */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UItemInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	UItemInventoryWidget* InventoryWidget;

	UPROPERTY()
	AItemActor* PendingItem;

	UFUNCTION(BlueprintCallable)
	void AddItem(UTexture2D* ItemIcon);

	void UpdateInventoryUI();

	void RequestPickup(AItemActor* Item, bool bIsWallet);

	UFUNCTION(BlueprintCallable)
	void ConfirmPickupYes();

	UFUNCTION(BlueprintCallable)
	void ConfirmPickupNo();
};
