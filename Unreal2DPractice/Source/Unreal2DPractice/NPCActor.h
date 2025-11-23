// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "PaperSpriteComponent.h"
#include "NPCActor.generated.h"

class UPaperFlipbookComponent;

/**
 * 
 */
UCLASS()
class UNREAL2DPRACTICE_API ANPCActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:
	ANPCActor();

	virtual void Interact_Implementation() override;
protected:
	
	virtual void BeginPlay() override;

private:
	bool bCanInteract;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperFlipbookComponent* Sprite;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* TalkWidgetComponent;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FText DialogueText;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> TalkWidgetClass;
	

};
