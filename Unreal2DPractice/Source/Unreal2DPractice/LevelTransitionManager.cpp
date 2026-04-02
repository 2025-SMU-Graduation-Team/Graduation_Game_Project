#include "LevelTransitionManager.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/PackageName.h"
#include "SubLevelTaskManager.h"
#include "TimerManager.h"

ALevelTransitionManager::ALevelTransitionManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentSubLevel = NAME_None;
	bIsTransitioning = false;
	PendingNextLevel = NAME_None;
	PendingPreviousLevel = NAME_None;
	bFinishTransitionQueued = false;
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
			PlayerToTeleport->bEnableMovement = true;
		}
		return;
	}

	bIsTransitioning = true;
	PendingPreviousLevel = CurrentSubLevel;
	PendingNextLevel = SanitizedNextLevel;
	PendingTeleportPlayer = PlayerToTeleport;
	PendingTeleportLocation = TeleportLocation;

	if (PendingTeleportPlayer)
	{
		PendingTeleportPlayer->bEnableMovement = false;

		if (UCharacterMovementComponent* MovementComponent = PendingTeleportPlayer->GetCharacterMovement())
		{
			MovementComponent->StopMovementImmediately();
		}
	}

	if (PendingNextLevel != NAME_None)
	{
		LoadLevel(PendingNextLevel);
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

	QueueFinishTransition();
}

void ALevelTransitionManager::QueueFinishTransition()
{
	if (bFinishTransitionQueued || !GetWorld())
	{
		return;
	}

	bFinishTransitionQueued = true;
	GetWorld()->GetTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			bFinishTransitionQueued = false;

			if (!bIsTransitioning)
			{
				return;
			}

			FinishTransition();
		})
	);
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
	bFinishTransitionQueued = false;
	bIsTransitioning = false;
}

bool ALevelTransitionManager::IsPersistentLevelTarget(FName LevelName) const
{
	if (LevelName.IsNone())
	{
		return true;
	}

	return NormalizeLevelNameForComparison(LevelName.ToString()) ==
		NormalizeLevelNameForComparison(GetPersistentLevelName().ToString());
}

FName ALevelTransitionManager::GetPersistentLevelName() const
{
	if (!GetWorld() || !GetWorld()->PersistentLevel)
	{
		return NAME_None;
	}

	return FName(*FPackageName::GetShortName(GetWorld()->PersistentLevel->GetOutermost()->GetName()));
}

FString ALevelTransitionManager::NormalizeLevelNameForComparison(const FString& LevelName) const
{
	FString NormalizedName = FPackageName::GetShortName(LevelName);

	if (NormalizedName.StartsWith(TEXT("UEDPIE_")))
	{
		int32 FirstUnderscoreIndex = INDEX_NONE;
		int32 SecondUnderscoreIndex = INDEX_NONE;

		if (NormalizedName.FindChar(TEXT('_'), FirstUnderscoreIndex))
		{
			SecondUnderscoreIndex = NormalizedName.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, FirstUnderscoreIndex + 1);
		}

		if (SecondUnderscoreIndex != INDEX_NONE && SecondUnderscoreIndex + 1 < NormalizedName.Len())
		{
			NormalizedName = NormalizedName.RightChop(SecondUnderscoreIndex + 1);
		}
	}

	return NormalizedName;
}
