// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetInteractionActor.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AWidgetInteractionActor::AWidgetInteractionActor()
{
}


void AWidgetInteractionActor::Interact()
{
	if (!PC || !WidgetClass) return;

	if (CurrentPanel && CurrentPanel->IsInViewport())
	{
		CurrentPanel->RemoveFromParent();
		CurrentPanel = nullptr;

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());

		return;
	}

	CurrentPanel = CreateWidget<UUserWidget>(PC, WidgetClass);
	if (CurrentPanel)
	{
		CurrentPanel->AddToViewport();

		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI());
	}
}