// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UISelectedManager.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API UUISelectedManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FName LastSelectedStation;
};
