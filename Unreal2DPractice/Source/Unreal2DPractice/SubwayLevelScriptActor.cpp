// Fill out your copyright notice in the Description page of Project Settings.


#include "SubwayLevelScriptActor.h"
#include "SubLevelTaskManager.h"

void ASubwayLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		USubLevelTaskManager::Get(World)->OnSubLevelEntered();
	}
}
