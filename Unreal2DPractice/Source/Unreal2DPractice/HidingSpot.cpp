// Fill out your copyright notice in the Description page of Project Settings.


#include "HidingSpot.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "PaperSpriteComponent.h"

#include "MyPaperCharacter.h"

// Sets default values
AHidingSpot::AHidingSpot()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(Root);

    TriggerBox->SetBoxExtent(FVector(40.f, 20.f, 100.f));
    TriggerBox->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);

    Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
    Sprite->SetupAttachment(Root);
    Sprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 숨는 오브젝트가 플레이어보다 “앞에” 보이도록 정렬 우선순위 높게
    Sprite->SetTranslucentSortPriority(10);

}

void AHidingSpot::BeginPlay()
{
    Super::BeginPlay();

    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(
            this, &AHidingSpot::OnTriggerBegin);

        TriggerBox->OnComponentEndOverlap.AddDynamic(
            this, &AHidingSpot::OnTriggerEnd);
    }
}

void AHidingSpot::OnTriggerBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
    if (!Player) return;

    Player->SetCanHide(this);

    UE_LOG(LogTemp, Warning, TEXT("HidingSpot: Player entered hide area"));
}

void AHidingSpot::OnTriggerEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
    if (!Player) return;

    Player->ClearCanHide(this);

    UE_LOG(LogTemp, Warning, TEXT("HidingSpot: Player left hide area"));
}


#if WITH_EDITOR
void AHidingSpot::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (Sprite && HideSpriteAsset)
    {
        Sprite->SetSprite(HideSpriteAsset);
    }
}
#endif