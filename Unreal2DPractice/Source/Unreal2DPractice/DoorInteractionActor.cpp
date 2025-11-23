// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionActor.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"

ADoorInteractionActor::ADoorInteractionActor()
{
}

void ADoorInteractionActor::Interact()
{
    if (TargetLevelName == "SongChaegang")
    {
        if (CachedPlayer)
        {
            CachedPlayer->SetActorLocation(TargetTeleportLocation);
        }
    }
    else
    {
        UGameplayStatics::LoadStreamLevel(this, TargetLevelName, true, false, FLatentActionInfo());
        if (CachedPlayer)
        {
            CachedPlayer->SetActorLocation(TargetTeleportLocation);
        }
    }
}
