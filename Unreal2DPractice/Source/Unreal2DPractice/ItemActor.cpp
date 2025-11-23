// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"

// Sets default values
AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = false;

    Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
    RootComponent = Sprite;

}

void AItemActor::Interact_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Item Interacted: %s"), *ItemName.ToString());

    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPaperCharacter::StaticClass()));
    if (!Player) return;

    if (ItemType == EItemType::Ghost)
    {
        UE_LOG(LogTemp, Error, TEXT("Ghost item interacted! Player dies."));
        Player->PlayDeath();
        Destroy();
        return;
    }

    Player->RequestItemPickup(this);

    //if (ItemType == EItemType::Wallet)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("Wallet picked ¡æ Convert to Card"));

    //    Player->AddItem(ItemIcon);
    //    Destroy();
    //    return;
    //}

    //if (ItemType == EItemType::Normal)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("Normal item picked."));

    //    Player->AddItem(ItemIcon);
    //    Destroy();
    //    return;
    //}
}


