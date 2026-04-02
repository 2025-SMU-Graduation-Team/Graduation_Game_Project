// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMessageWidget.h"

void UPlayerMessageWidget::SetMessage(const FString& Msg)
{
	if (MessageText)
	{
		MessageText->SetText(FText::FromString(Msg));
	}
}