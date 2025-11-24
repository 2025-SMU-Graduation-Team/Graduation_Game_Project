// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "ItemInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API UItemInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateInventory(const TArray<UTexture2D*>& Items);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowConfirmPopup(UTexture2D* ItemIcon);

	UFUNCTION(BlueprintImplementableEvent)
	void HideConfirmPopup();

protected:
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* InventoryBox;
	
};
