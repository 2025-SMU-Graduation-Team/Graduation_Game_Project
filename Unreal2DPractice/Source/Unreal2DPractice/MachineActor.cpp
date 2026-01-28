// Fill out your copyright notice in the Description page of Project Settings.


#include "MachineActor.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMachineActor::AMachineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AMachineActor::BeginPlay()
{
	if (WBPMachine)
	{
        PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}
}

void AMachineActor::Interact()
{
    if (!WBPMachine || !PC) return;

    if (!CurrentPanel)
    {
        CurrentPanel = CreateWidget<UUserWidget>(PC, WBPMachine);

        if (CurrentPanel)
        {
            CurrentPanel->AddToViewport();

            PC->bShowMouseCursor = true;
            PC->SetInputMode(FInputModeUIOnly());
        }
    }
}


