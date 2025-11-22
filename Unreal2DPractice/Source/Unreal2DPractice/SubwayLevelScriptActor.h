// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "SubwayLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API ASubwayLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};
