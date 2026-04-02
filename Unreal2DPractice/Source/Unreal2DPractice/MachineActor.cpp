// Fill out your copyright notice in the Description page of Project Settings.


#include "MachineActor.h"
#include "Blueprint/UserWidget.h"
#include "MyPaperCharacter.h"
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

    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(CachedPlayer);
    if (!Player) return;

    if (CurrentPanel)
    {
        Player->bEnableMovement = true; 

        CurrentPanel->RemoveFromParent();
        CurrentPanel = nullptr;

        PC->bShowMouseCursor = false;

        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);

        return;
    }

    CurrentPanel = CreateWidget<UUserWidget>(PC, WBPMachine);

    if (CurrentPanel)
    {
        Player->bEnableMovement = false;

        // Blueprint Widget으로 캐스팅
        UObject* WidgetObj = CurrentPanel;

        // Blueprint에서 변수 설정 가능하게
        UFunction* Func = WidgetObj->FindFunction(FName("SetTurnstileRef"));

        if (Func)
        {
            struct
            {
                AActor* Turnstile;
            } Params;

            Params.Turnstile = TurnstileRef;

            WidgetObj->ProcessEvent(Func, &Params);
        }

        CurrentPanel->AddToViewport();
        PC->bShowMouseCursor = true;

        FInputModeGameAndUI InputMode; 
        InputMode.SetWidgetToFocus(CurrentPanel->TakeWidget()); 
        InputMode.SetHideCursorDuringCapture(false); 

        PC->SetInputMode(InputMode);
    }
}


