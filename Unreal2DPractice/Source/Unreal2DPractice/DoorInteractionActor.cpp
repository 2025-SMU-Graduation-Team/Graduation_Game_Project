// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionActor.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"
#include "LevelTransitionManager.h"
#include "MyGameInstance.h"
#include "AudioManager.h"
#include "GameSFXData.h"

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

	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());

	if (GI && GI->SFXData && GI->SFXData->DoorOpen)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			GI->SFXData->DoorOpen,
			GetActorLocation()
		);
	}

	Manager->ChangeSubLevel(TargetLevelName, CachedPlayer, TargetTeleportLocation);
}
