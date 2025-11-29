// Fill out your copyright notice in the Description page of Project Settings.


#include "HidingSpot.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"

// Sets default values
AHidingSpot::AHidingSpot()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Block);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 플레이어와 겹침

    Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
    Sprite->SetupAttachment(RootComponent);

    // 숨는 오브젝트가 플레이어보다 “앞에” 보이도록 정렬 우선순위 높게
    Sprite->SetTranslucentSortPriority(10);

}
