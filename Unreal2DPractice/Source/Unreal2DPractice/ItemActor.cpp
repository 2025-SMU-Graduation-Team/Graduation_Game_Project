// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"

// Sets default values
AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = false;

    Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
    Sprite->SetupAttachment(RootComponent);

    Sprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::Interact()
{
    UE_LOG(LogTemp, Warning, TEXT("Item Interacted: %s"), *ItemName.ToString());

    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(CachedPlayer);
    if (!Player) return;

    Player->bEnableMovement = false;
    Player->Inventory->RequestPickup(this);

    /*switch (ItemType)
    {
    case EItemType::Ghost:
        Player->PlayDeath();
        Destroy();
        return;

    case EItemType::Wallet:
        Player->Inventory->RequestPickup(this); 
        break;

    case EItemType::Normal:
    default:
        Player->Inventory->RequestPickup(this); 
        break;
    }*/
}


