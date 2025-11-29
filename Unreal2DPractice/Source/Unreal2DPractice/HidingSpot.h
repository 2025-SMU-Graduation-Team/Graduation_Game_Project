// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HidingSpot.generated.h"

class UBoxComponent;
class UPaperFlipbookComponent; // 2D 스프라이트

UCLASS()
class UNREAL2DPRACTICE_API AHidingSpot : public AActor
{
	GENERATED_BODY()
	
public:	
	AHidingSpot();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperFlipbookComponent* Sprite;

public:	
	UBoxComponent* GetTriggerBox() const { return TriggerBox; }
};
