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

	UE_LOG(LogTemp, Log, TEXT("AWidgetInteractionActor is interacted"));


	if (CurrentPanel && CurrentPanel->IsInViewport())
	{
		UE_LOG(LogTemp, Log, TEXT("Panel is opened. Closing the panel"));

		CurrentPanel->RemoveFromParent();
		CurrentPanel = nullptr;

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());

		return;
	}

	CurrentPanel = CreateWidget<UUserWidget>(PC, WidgetClass);
	if (CurrentPanel)
	{
		UE_LOG(LogTemp, Log, TEXT("Panel is closed. Openning the panel"));
		CurrentPanel->AddToViewport();

		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI());
	}
}