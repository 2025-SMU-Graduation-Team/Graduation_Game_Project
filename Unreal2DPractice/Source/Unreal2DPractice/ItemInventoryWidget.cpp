// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInventoryWidget.h"

void UItemInventoryWidget::UpdateInventory(const TArray<UTexture2D*>& Items)
{
	if (!InventoryBox) return;

	int32 ChildCount = InventoryBox->GetChildrenCount();
	for (int32 i = 0; i < ChildCount; i++)
	{
		UImage* SlotImage = Cast<UImage>(InventoryBox->GetChildAt(i));
		if (!SlotImage) continue;

		if (i < Items.Num() && Items[i] != nullptr)
		{
			FSlateBrush Brush;
			Brush.SetResourceObject(Items[i]);
			Brush.ImageSize = FVector2D(60.f, 60.f);
			SlotImage->SetBrush(Brush);
		}
		else
		{
			FSlateBrush EmptyBrush;
			SlotImage->SetBrush(EmptyBrush);
		}
	}
}
