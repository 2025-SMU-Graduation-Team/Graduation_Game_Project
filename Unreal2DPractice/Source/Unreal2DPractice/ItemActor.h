// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EItemType.h"
#include "PaperSpriteComponent.h"
#include "InteractableActor.h"
#include "ItemActor.generated.h"

UCLASS()
class UNREAL2DPRACTICE_API AItemActor : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();

	virtual void Interact() override;

protected:

public:	
	UPROPERTY(VisibleAnywhere)
	UPaperSpriteComponent* Sprite;

	UPROPERTY(EditAnywhere, Category = "Item")
	FName ItemName;

	UPROPERTY(EditAnywhere, Category = "Item")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, Category = "Item")
	FText ItemDescription;
};
