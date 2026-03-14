// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingMonster.h"
#include "MyPaperCharacter.h"
#include "EndingChaseManager.h"
#include "Components/BoxComponent.h"

AEndingMonster::AEndingMonster()
{
	PrimaryActorTick.bCanEverTick = true;
	MoveSpeed = FastSpeed;
}

void AEndingMonster::BeginPlay()
{
	Super::BeginPlay();
}

void AEndingMonster::Tick(float DeltaTime)
{
	if (bEndTriggered)
	{
		return;
	}

	if (TryKillPlayerAhead())
	{
		return;
	}

	CheckTurnPoint();
	CheckEndPoint();

	if (HandleObstacle(DeltaTime))
	{
		return;
	}

	Super::Tick(DeltaTime);
}

void AEndingMonster::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnHitBoxOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (bHasKilledPlayer) return;

	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player) return;

	TryKillPlayer(Player);
}

void AEndingMonster::CheckTurnPoint()
{
	const float DistanceX = FMath::Abs(GetActorLocation().X - TurnLocation.X);

	if (DistanceX < 30.f)
	{
		if (!bTurnedAtPoint)
		{
			SetMoveDirection(-1.f * MoveDirection);
			bTurnedAtPoint = true;
		}
	}
	else
	{
		bTurnedAtPoint = false;
	}
}

void AEndingMonster::CheckEndPoint()
{
	const float DistanceX = FMath::Abs(GetActorLocation().X - EndLocation.X);

	if (DistanceX < 30.f)
	{
		bEndTriggered = true;

		if (Manager)
		{
			Manager->NotifyReachedEnd(this);
		}
	}
}

bool AEndingMonster::HandleObstacle(float DeltaTime)
{
	FHitResult Hit;
	const bool bBlocked = IsFrontBlocked(Hit);

	if (!bBreakingObstacle)
	{
		if (!bBlocked)
		{
			CurrentObstacle = nullptr;
			return false;
		}

		CurrentObstacle = Hit.GetActor();
		bBreakingObstacle = true;
		ObstacleTimer = 0.f;
		SetState(EMonsterState::Idle);
		return true;
	}

	SetState(EMonsterState::Idle);
	ObstacleTimer += DeltaTime;

	if (ObstacleTimer < WaitBeforeBreak)
	{
		return true;
	}

	if (CurrentObstacle && IsValid(CurrentObstacle))
	{
		const bool bStillBlocked = bBlocked && Hit.GetActor() == CurrentObstacle;
		if (bStillBlocked || ObstacleTimer >= WaitBeforeBreak + BreakDuration)
		{
			CurrentObstacle->Destroy();
			CurrentObstacle = nullptr;
			bBreakingObstacle = false;
			ObstacleTimer = 0.f;
			return true;
		}

		return true;
	}

	bBreakingObstacle = false;
	ObstacleTimer = 0.f;
	return true;
}

bool AEndingMonster::TryKillPlayerAhead()
{
	if (bHasKilledPlayer)
	{
		return true;
	}

	if (HitBox)
	{
		TArray<AActor*> OverlappingActors;
		HitBox->GetOverlappingActors(OverlappingActors, AMyPaperCharacter::StaticClass());

		for (AActor* Actor : OverlappingActors)
		{
			if (AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(Actor))
			{
				if (TryKillPlayer(Player))
				{
					return true;
				}
			}
		}
	}

	const FVector Start = HitBox ? HitBox->GetComponentLocation() : GetActorLocation();
	const FVector End = Start + FVector(MoveDirection * PlayerDetectDistance, 0.f, 0.f);
	const FCollisionShape Box = FCollisionShape::MakeBox(FVector(45.f, 60.f, 120.f));

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	TArray<FHitResult> Hits;
	if (!GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjParams, Box, Params))
	{
		return false;
	}

	for (const FHitResult& Hit : Hits)
	{
		if (AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(Hit.GetActor()))
		{
			if (TryKillPlayer(Player))
			{
				return true;
			}
		}
	}

	return false;
}

bool AEndingMonster::TryKillPlayer(AMyPaperCharacter* Player)
{
	if (!Player || bHasKilledPlayer || Player->bIsHidden)
	{
		return false;
	}

	bHasKilledPlayer = true;
	Player->PlayDeath();
	SetActorTickEnabled(false);
	Destroy();
	return true;
}

bool AEndingMonster::IsFrontBlocked(FHitResult& OutHit) const
{
	const FVector Start = HitBox ? HitBox->GetComponentLocation() : GetActorLocation();
	const FVector End = Start + FVector(MoveDirection * 95.f, 0.f, 0.f);

	const FVector HalfSize(36.f, 48.f, 100.f);
	const FCollisionShape Box = FCollisionShape::MakeBox(HalfSize);

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	TArray<FHitResult> Hits;
	if (!GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjParams, Box, Params))
	{
		return false;
	}

	float ClosestDistance = TNumericLimits<float>::Max();
	bool bFoundObstacle = false;

	for (const FHitResult& Hit : Hits)
	{
		if (!IsBreakableHit(Hit))
		{
			continue;
		}

		const AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			continue;
		}

		const float ForwardDistance = (HitActor->GetActorLocation().X - GetActorLocation().X) * MoveDirection;
		if (ForwardDistance < 25.f || ForwardDistance > 110.f)
		{
			continue;
		}

		if (ForwardDistance < ClosestDistance)
		{
			ClosestDistance = ForwardDistance;
			OutHit = Hit;
			bFoundObstacle = true;
		}
	}

	return bFoundObstacle;
}

bool AEndingMonster::IsBreakableHit(const FHitResult& Hit) const
{
	const AActor* HitActor = Hit.GetActor();
	const UPrimitiveComponent* HitComp = Hit.GetComponent();

	return (HitActor && HitActor->ActorHasTag(TEXT("Breakable")))
		|| (HitComp && HitComp->ComponentHasTag(TEXT("Breakable")));
}

void AEndingMonster::SetMoveDirection(float Dir)
{
	MoveDirection = Dir;
	SetMoveDirectionX(Dir);
}

void AEndingMonster::SetEndLocation(const FVector& InLocation)
{
	EndLocation = InLocation;
}

void AEndingMonster::SetTurnLocation(const FVector& InLocation)
{
	TurnLocation = InLocation;
}

void AEndingMonster::SetManager(AEndingChaseManager* InManager)
{
	Manager = InManager;
}
