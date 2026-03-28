#include "LevelTransitionManager.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"
#include "Engine/LevelStreaming.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SubLevelTaskManager.h"

ALevelTransitionManager::ALevelTransitionManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentSubLevel = NAME_None;
	bIsTransitioning = false;
	PendingNextLevel = NAME_None;
	PendingPreviousLevel = NAME_None;
}

void ALevelTransitionManager::BeginPlay()
{
	Super::BeginPlay();

	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &ALevelTransitionManager::OnLevelLoaded);
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

void ALevelTransitionManager::ChangeSubLevel(FName NextLevel, AMyPaperCharacter* PlayerToTeleport, const FVector& TeleportLocation)
{
	if (bIsTransitioning)
	{
		return;
	}

	if (CurrentSubLevel == NextLevel)
	{
		if (PlayerToTeleport)
		{
			if (UCharacterMovementComponent* MovementComponent = PlayerToTeleport->GetCharacterMovement())
			{
				MovementComponent->StopMovementImmediately();
			}

			PlayerToTeleport->SetActorLocation(
				TeleportLocation,
				false,
				nullptr,
				ETeleportType::TeleportPhysics
			);
			PlayerToTeleport->bEnableMovement = true;
		}
		return;
	}

	bIsTransitioning = true;
	PendingPreviousLevel = CurrentSubLevel;
	PendingNextLevel = NextLevel;
	PendingTeleportPlayer = PlayerToTeleport;
	PendingTeleportLocation = TeleportLocation;

	if (NextLevel != NAME_None)
	{
		LoadLevel(NextLevel);
		return;
	}

	FinishTransition();
}

void ALevelTransitionManager::OnLevelLoaded(ULevel* LoadedLevel, UWorld* World)
{
	if (!bIsTransitioning || World != GetWorld() || !LoadedLevel || PendingNextLevel.IsNone())
	{
		return;
	}

	const UPackage* LevelPackage = LoadedLevel->GetPackage();
	if (!LevelPackage)
	{
		return;
	}

	if (!LevelPackage->GetName().Contains(PendingNextLevel.ToString()))
	{
		return;
	}

	FinishTransition();
}

void ALevelTransitionManager::FinishTransition()
{
	const FName PreviousSubLevel = PendingPreviousLevel;
	CurrentSubLevel = PendingNextLevel;

	if (PendingTeleportPlayer)
	{
		if (UCharacterMovementComponent* MovementComponent = PendingTeleportPlayer->GetCharacterMovement())
		{
			MovementComponent->StopMovementImmediately();
		}

		PendingTeleportPlayer->SetActorLocation(
			PendingTeleportLocation,
			false,
			nullptr,
			ETeleportType::TeleportPhysics
		);
		PendingTeleportPlayer->bEnableMovement = true;
	}

	if (PreviousSubLevel != NAME_None && PreviousSubLevel != CurrentSubLevel)
	{
		UnloadLevel(PreviousSubLevel);
	}

	if (CurrentSubLevel == FName(TEXT("Subway")))
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (USubLevelTaskManager* TaskManager = GI->GetSubsystem<USubLevelTaskManager>())
			{
				TaskManager->OnSubLevelEntered();
			}
		}
	}

	PendingPreviousLevel = NAME_None;
	PendingNextLevel = NAME_None;
	PendingTeleportPlayer = nullptr;
	PendingTeleportLocation = FVector::ZeroVector;
	bIsTransitioning = false;
}
