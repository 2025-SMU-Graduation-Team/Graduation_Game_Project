// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCActor.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "NPCWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "MyPaperCharacter.h"
#include "MyGameInstance.h"

ANPCActor::ANPCActor()
{
	PrimaryActorTick.bCanEverTick = false;

    Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
    Sprite->SetupAttachment(RootComponent);

	TalkWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TalkWidget"));
	TalkWidgetComponent->SetupAttachment(RootComponent);
	TalkWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

	bCanInteract = false;
}

	void ANPCActor::BeginPlay()
	{
		Super::BeginPlay();
	}

	void ANPCActor::KillPlayer()
	{
		AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (Player)
	{
		if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
		{
			GI->LastDeathCause = EDeathCause::NPC;
		}

		Player->PlayDeath();
	}
}

void ANPCActor::Interact()
{
	UE_LOG(LogTemp, Log, TEXT("Interact NPC"));
	UNPCWidget* Widget = Cast<UNPCWidget>(TalkWidgetComponent->GetUserWidgetObject());

	if (Widget)
	{
		Widget->ShowDialoguePopup(DialogueText, bKillPlayer);

		if (bKillPlayer)
		{
			FTimerHandle KillTimerHandle;

			GetWorld()->GetTimerManager().SetTimer(
				KillTimerHandle,
				this,
				&ANPCActor::KillPlayer,
				2.0f,
				false
			);
		}
	}
}
