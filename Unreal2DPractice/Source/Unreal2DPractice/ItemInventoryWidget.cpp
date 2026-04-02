// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInventoryWidget.h"

void UItemInventoryWidget::UpdateInventory(const TArray<FInventoryItem>& Items)
{
	if (!InventoryBox) return;

	int32 ChildCount = InventoryBox->GetChildrenCount();
	for (int32 i = 0; i < ChildCount; i++)
	{
		UOverlay* ItemSlot = Cast<UOverlay>(InventoryBox->GetChildAt(i));
		if (!ItemSlot) continue;

		UImage* ItemSlotImage = Cast<UImage>(ItemSlot->GetChildAt(0));
		if (!ItemSlotImage) continue;

		FSlateBrush Brush;
		Brush.ImageSize = FVector2D(100.f, 100.f);

		if (i < Items.Num() && Items[i].Icon)
		{
			Brush.SetResourceObject(Items[i].Icon);
		}
		else
		{
			Brush.SetResourceObject(EmptySlotTexture);
		}

		ItemSlotImage->SetBrush(Brush);
	}
}
