#include "VerticalTraversalActor.h"

#include "LevelTransitionManager.h"
#include "MyPaperCharacter.h"
#include "PaperFlipbook.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

AVerticalTraversalActor::AVerticalTraversalActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bAllowEnterKeyInteraction = true;
	AlternateInteractKey = FText::FromString("W");
	InteractText = FText::FromString("Climb");
}

void AVerticalTraversalActor::Interact()
{
	if (bIsTraversing || !CachedPlayer)
	{
		return;
	}

	TraversingPlayer = CachedPlayer;
	TraversalStartLocation = TraversingPlayer->GetActorLocation();
	TraversalElapsedTime = 0.0f;
	bIsTraversing = true;

	TraversingPlayer->bEnableMovement = false;
	TraversingPlayer->GetCharacterMovement()->StopMovementImmediately();

	if (ClimbAnimation)
	{
		TraversingPlayer->SetForcedFlipbook(ClimbAnimation);
	}

	const float SafeDuration = FMath::Max(TraversalDuration, 0.0f);
	if (SafeDuration <= KINDA_SMALL_NUMBER)
	{
		FinishTraversal();
		return;
	}

	GetWorldTimerManager().SetTimer(
		TraversalTimerHandle,
		this,
		&AVerticalTraversalActor::FinishTraversal,
		SafeDuration,
		false
	);
}

void AVerticalTraversalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsTraversing)
	{
		return;
	}

	UpdateTraversalPosition(DeltaTime);
}

void AVerticalTraversalActor::UpdateTraversalPosition(float DeltaTime)
{
	if (!TraversingPlayer)
	{
		return;
	}

	const float SafeDuration = FMath::Max(TraversalDuration, KINDA_SMALL_NUMBER);
	TraversalElapsedTime = FMath::Min(TraversalElapsedTime + DeltaTime, SafeDuration);

	const float Alpha = TraversalElapsedTime / SafeDuration;
	const float TargetZ = FMath::Lerp(
		TraversalStartLocation.Z,
		TargetTeleportLocation.Z,
		FMath::Clamp(Alpha * VisualClimbAlpha, 0.0f, 1.0f)
	);

	FVector NewLocation = TraversingPlayer->GetActorLocation();
	NewLocation.Z = TargetZ;
	TraversingPlayer->SetActorLocation(NewLocation);
}

void AVerticalTraversalActor::FinishTraversal()
{
	GetWorldTimerManager().ClearTimer(TraversalTimerHandle);

	if (!TraversingPlayer)
	{
		bIsTraversing = false;
		return;
	}

	if (!TargetLevelName.IsNone())
	{
		if (ALevelTransitionManager* Manager = ALevelTransitionManager::Get(GetWorld()))
		{
			Manager->ChangeSubLevel(TargetLevelName, TraversingPlayer, TargetTeleportLocation);
		}
	}
	else
	{
		TraversingPlayer->SetActorLocation(TargetTeleportLocation);
	}
	TraversingPlayer->ClearForcedFlipbook();

	if (bRestoreMovementAfterTraversal)
	{
		TraversingPlayer->bEnableMovement = true;
	}

	TraversingPlayer->GetCharacterMovement()->StopMovementImmediately();
	TraversingPlayer = nullptr;
	TraversalElapsedTime = 0.0f;
	bIsTraversing = false;
}
