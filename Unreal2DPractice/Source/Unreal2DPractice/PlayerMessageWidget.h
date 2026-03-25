// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API UPlayerMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MessageText;

	UFUNCTION(BlueprintCallable)
	void SetMessage(const FString& Msg);

};
