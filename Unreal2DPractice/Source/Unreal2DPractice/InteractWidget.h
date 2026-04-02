// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API UInteractWidget : public UUserWidget
{
	GENERATED_BODY()
	
	public:
	void SetInteractInfo(const FText& InKey, const FText& InText);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KeyTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractTextBlock;
};
