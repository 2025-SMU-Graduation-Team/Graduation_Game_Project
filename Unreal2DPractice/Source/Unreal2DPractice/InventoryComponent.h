// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EItemType.h"
#include "InventoryComponent.generated.h"

// Forward Declaration
class UTexture2D;
class UItemInventoryWidget;
class AItemActor;

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemType ItemType;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnEquipItemChanged,
	EItemType,
	NewItemType
);

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
	TArray<FInventoryItem> Items;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 MaxInventorySize = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SelectedInvenIndex = INDEX_NONE;

	UPROPERTY(BlueprintAssignable)
	FOnEquipItemChanged OnEquipItemChanged;

	UFUNCTION(BlueprintCallable)
	bool IsInventoryFull() const;

	/** UI Widget */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UItemInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	UItemInventoryWidget* InventoryWidget;

	UPROPERTY()
	AItemActor* PendingItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EItemType EquippedItemType;

	UFUNCTION(BlueprintCallable)
	void SelectSlot(int32 Index);

	UFUNCTION(BlueprintCallable)
	void UseSelectedItem();

	UFUNCTION(BlueprintCallable)
	void AddItem(AItemActor* ItemActor);

	void UpdateInventoryUI();

	void RequestPickup(AItemActor* Item, bool bIsWallet);

	UFUNCTION(BlueprintCallable)
	void ConfirmPickupYes();

	UFUNCTION(BlueprintCallable)
	void ConfirmPickupNo();

private:
	UPROPERTY(EditAnywhere, Category = "Card")
	UTexture2D* CardIcon;
};
