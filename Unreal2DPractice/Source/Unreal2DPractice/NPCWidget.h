// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API UNPCWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDialoguePopup(const FText& DialogueText, bool IsKillPlayer);
};
