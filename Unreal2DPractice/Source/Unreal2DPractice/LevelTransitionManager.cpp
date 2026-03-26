#include "LevelTransitionManager.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/PackageName.h"
#include "SubLevelTaskManager.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "TimerManager.h"

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
	if (ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(this, LevelName))
	{
		StreamingLevel->bShouldBlockOnLoad = true;
		StreamingLevel->SetShouldBeLoaded(true);
		StreamingLevel->SetShouldBeVisible(true);
		return;
	}

	UGameplayStatics::LoadStreamLevel(
		this,
		LevelName,
		true,
		true,
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

	const bool bTargetPersistentLevel = IsPersistentLevelTarget(NextLevel);
	const FName SanitizedNextLevel = bTargetPersistentLevel ? NAME_None : NextLevel;

	if (CurrentSubLevel == SanitizedNextLevel)
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
			PlayerToTeleport->RefreshAfterLevelTransition();
		}
		return;
	}

	bIsTransitioning = true;
	BeginFadeTransition(SanitizedNextLevel, PlayerToTeleport, TeleportLocation);
}

void ALevelTransitionManager::BeginFadeTransition(FName NextLevel, AMyPaperCharacter* PlayerToTeleport, const FVector& TeleportLocation)
{
	if (PlayerToTeleport)
	{
		PlayerToTeleport->bEnableMovement = false;

		if (UCharacterMovementComponent* MovementComponent = PlayerToTeleport->GetCharacterMovement())
		{
			MovementComponent->StopMovementImmediately();
		}
	}

	APlayerController* PC = GetPrimaryPlayerController();
	if (!PC || !PC->PlayerCameraManager || TransitionFadeDuration <= 0.f)
	{
		if (NextLevel != NAME_None)
		{
			LoadLevel(NextLevel);
		}

		FinishTransition(NextLevel, PlayerToTeleport, TeleportLocation);
		return;
	}

	PC->PlayerCameraManager->StartCameraFade(
		0.f,
		1.f,
		TransitionFadeDuration,
		FLinearColor::Black,
		false,
		true
	);

	FTimerHandle FadeTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		FadeTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this, NextLevel, PlayerToTeleport, TeleportLocation]()
		{
			if (NextLevel != NAME_None)
			{
				LoadLevel(NextLevel);
			}

			FinishTransition(NextLevel, PlayerToTeleport, TeleportLocation);
		}),
		TransitionFadeDuration,
		false
	);
}

void ALevelTransitionManager::FinishTransition(FName NextLevel, AMyPaperCharacter* PlayerToTeleport, const FVector& TeleportLocation)
{
	const FName PreviousSubLevel = CurrentSubLevel;

	if (UWorld* World = GetWorld())
	{
		World->FlushLevelStreaming(EFlushLevelStreamingType::Full);
	}

	CurrentSubLevel = NextLevel;

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
		PlayerToTeleport->RefreshAfterLevelTransition();
		PlayerToTeleport->bEnableMovement = true;
	}

	if (NextLevel == FName(TEXT("Subway")))
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (USubLevelTaskManager* TaskManager = GI->GetSubsystem<USubLevelTaskManager>())
			{
				TaskManager->OnSubLevelEntered();
			}
		}
	}

	if (PreviousSubLevel != NAME_None && PreviousSubLevel != NextLevel)
	{
		UnloadLevel(PreviousSubLevel);

		if (UWorld* World = GetWorld())
		{
			World->FlushLevelStreaming(EFlushLevelStreamingType::Visibility);
		}
	}

	if (APlayerController* PC = GetPrimaryPlayerController())
	{
		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraFade(
				1.f,
				0.f,
				TransitionFadeDuration,
				FLinearColor::Black,
				false,
				false
			);
		}
	}

	bIsTransitioning = false;
}

bool ALevelTransitionManager::IsPersistentLevelTarget(FName LevelName) const
{
	return LevelName.IsNone() || LevelName == GetPersistentLevelName();
}

FName ALevelTransitionManager::GetPersistentLevelName() const
{
	if (!GetWorld() || !GetWorld()->PersistentLevel)
	{
		return NAME_None;
	}

	return FName(*FPackageName::GetShortName(GetWorld()->PersistentLevel->GetOutermost()->GetName()));
}

APlayerController* ALevelTransitionManager::GetPrimaryPlayerController() const
{
	return GetWorld() ? UGameplayStatics::GetPlayerController(GetWorld(), 0) : nullptr;
}
