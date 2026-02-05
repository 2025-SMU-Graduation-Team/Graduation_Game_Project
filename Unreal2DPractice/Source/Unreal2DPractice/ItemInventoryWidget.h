// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "InventoryComponent.h" 
#include "ItemInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API UItemInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateInventory(const TArray<FInventoryItem>& Items);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowConfirmPopup(UTexture2D* ItemIcon);

	UFUNCTION(BlueprintImplementableEvent)
	void HideConfirmPopup();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowInventoryFullPopup();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowItemInfoPopup(const FText& ItemName, const FText& ItemDescription);

	UFUNCTION(BlueprintImplementableEvent)
	void HideItemInfoPopup();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowBorder(int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
	void HideAllBorder();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowEquippedItem(UTexture2D* ItemIcon);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	UTexture2D* EmptySlotTexture;

protected:
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* InventoryBox;
	
};
