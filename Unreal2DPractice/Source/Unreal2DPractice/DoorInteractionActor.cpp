// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionActor.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"
#include "LevelTransitionManager.h"

ADoorInteractionActor::ADoorInteractionActor()
{
}

void ADoorInteractionActor::Interact()
{
	ALevelTransitionManager* Manager = ALevelTransitionManager::Get(GetWorld());

	if (!Manager || !CachedPlayer)
	{
		return;
	}

	Manager->ChangeSubLevel(TargetLevelName);

	CachedPlayer->SetActorLocation(TargetTeleportLocation);
}
