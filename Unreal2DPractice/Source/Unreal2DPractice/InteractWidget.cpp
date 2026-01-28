// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractWidget.h"
#include "Components/TextBlock.h"

void UInteractWidget::SetInteractInfo(const FText& InKey, const FText& InText)
{
	if (KeyTextBlock)
	{
		KeyTextBlock->SetText(InKey);
	}

	if (InteractTextBlock)
	{
		InteractTextBlock->SetText(InText);
	}
}