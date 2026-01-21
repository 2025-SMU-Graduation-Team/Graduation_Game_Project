#include "LevelTransitionManager.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"
#include "Engine/LevelStreaming.h"
#include "EngineUtils.h"

ALevelTransitionManager::ALevelTransitionManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentSubLevel = NAME_None;
	bIsTransitioning = false;
}

void ALevelTransitionManager::BeginPlay()
{
	Super::BeginPlay();
}

ALevelTransitionManager* ALevelTransitionManager::Get(UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<ALevelTransitionManager> It(World); It; ++It)
	{
		return *It;
	}

	return nullptr;
}

void ALevelTransitionManager::LoadLevel(FName LevelName)
{
	UGameplayStatics::LoadStreamLevel(
		this,
		LevelName,
		true,
		false,
		FLatentActionInfo()
	);
}

void ALevelTransitionManager::UnloadLevel(FName LevelName)
{
	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(this, LevelName);

	if (!StreamingLevel)
	{
		return;
	}

	StreamingLevel->SetShouldBeLoaded(false);
	StreamingLevel->SetShouldBeVisible(false);
}

void ALevelTransitionManager::ChangeSubLevel(FName NextLevel)
{
	if (bIsTransitioning)
	{
		return;
	}

	bIsTransitioning = true;

	if (CurrentSubLevel != NAME_None && CurrentSubLevel != NextLevel)
	{
		UnloadLevel(CurrentSubLevel);
	}

	if (NextLevel != NAME_None)
	{
		LoadLevel(NextLevel);
	}

	CurrentSubLevel = NextLevel;
	bIsTransitioning = false;
}
