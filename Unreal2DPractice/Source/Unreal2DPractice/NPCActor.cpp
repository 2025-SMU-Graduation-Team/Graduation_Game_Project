// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCActor.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "TimerManager.h"

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

    TalkWidgetComponent->SetVisibility(false);
    if (!TalkWidgetClass)
    {
        return;
    }

    TalkWidgetComponent->SetWidgetClass(TalkWidgetClass);
    TalkWidgetComponent->InitWidget();
    UUserWidget* Widget = TalkWidgetComponent->GetUserWidgetObject();

    if (!Widget)
    {
        return;
    }

    UTextBlock* TextBlock = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("DialogueText")));

    if (!TextBlock)
    {
        return;
    }

    TextBlock->SetText(DialogueText);
}

void ANPCActor::Interact()
{
    TalkWidgetComponent->SetVisibility(true);
    UUserWidget* Widget = TalkWidgetComponent->GetUserWidgetObject();

    if (Widget)
    {
        UTextBlock* TextBlock = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("DialogueText")));

        if (TextBlock)
        {
            TextBlock->SetText(DialogueText);
        }
    }

    FTimerHandle HideTimer;
    GetWorld()->GetTimerManager().SetTimer(
        HideTimer,
        [this]()
        {
            TalkWidgetComponent->SetVisibility(false);
        },
        1.5f,
        false
    );
}