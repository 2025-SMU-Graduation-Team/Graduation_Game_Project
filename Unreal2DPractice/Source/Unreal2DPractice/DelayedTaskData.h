// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Components/Button.h"
#include "DelayedTaskData.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API UDelayedTaskData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	TSoftObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform TargetTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseSmoothMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Delay = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAnswer = false;
};
