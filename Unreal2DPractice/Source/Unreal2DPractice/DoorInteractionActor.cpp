// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionActor.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"

ADoorInteractionActor::ADoorInteractionActor()
{
}

void ADoorInteractionActor::Interact()
{
    UE_LOG(LogTemp, Log, TEXT("Door Interacted!"));

    UGameplayStatics::LoadStreamLevel(this, TargetLevelName, true, true, FLatentActionInfo());

    if (CachedPlayer)
    {
        CachedPlayer->SetActorLocation(TargetTeleportLocation);
    }
}
