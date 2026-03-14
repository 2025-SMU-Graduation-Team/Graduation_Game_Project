// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

class UGameSFXData;
class UGameBGMData;

UCLASS()
class UNREAL2DPRACTICE_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> DefaultInteractWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	UGameSFXData* SFXData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	UGameBGMData* BGMData;
};
