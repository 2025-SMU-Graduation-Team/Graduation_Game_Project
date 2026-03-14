// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnstileActor.h"
#include "MyPaperCharacter.h"

// Sets default values
ATurnstileActor::ATurnstileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATurnstileActor::Interact()
{
    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

    if (!Player) return;

    if (Player->EquippedItem == EItemType::Card)  
    {
        SetActorEnableCollision(false);
        UE_LOG(LogTemp, Warning, TEXT("Gate opened"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Need a card"));
    }
}

// Called when the game starts or when spawned
void ATurnstileActor::BeginPlay()
{
	Super::BeginPlay();
	
}



