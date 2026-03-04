// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingMonster.h"
#include "MyPaperCharacter.h"
#include "EndingChaseManager.h"

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
	Super::Tick(DeltaTime);

	if (bEndTriggered) return;

	CheckTurnPoint();
	CheckEndPoint();
	HandleObstacle(DeltaTime);
}

void AEndingMonster::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnHitBoxOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (bHasKilledPlayer) return;

	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player) return;

	bHasKilledPlayer = true;

	Player->PlayDeath();

	SetActorTickEnabled(false);

	Destroy();
}

void AEndingMonster::CheckTurnPoint()
{
	const float DistanceX = FMath::Abs(GetActorLocation().X - TurnLocation.X);

	if (DistanceX < 10.f)
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

void AEndingMonster::HandleObstacle(float DeltaTime)
{
	FHitResult Hit;

	if (!bBreakingObstacle)
	{
		if (IsFrontBlocked(Hit))
		{
			bBreakingObstacle = true;
			BreakTime = 0.f;
			BreakDirection = MoveDirection;
			CurrentObstacle = Hit.GetActor();

			return;
		}

		AddMovementInput(FVector(MoveDirection, 0.f, 0.f), 1.f);
		return;
	}

	BreakTime += DeltaTime;

	if (BreakTime >= BreakDuration)
	{
		if (CurrentObstacle && IsValid(CurrentObstacle))
		{
			CurrentObstacle->Destroy(); // ÇÑ °³¾¿ ÆÄ±«
		}

		bBreakingObstacle = false;
		BreakTime = 0.f;
		CurrentObstacle = nullptr;
	}
}

bool AEndingMonster::IsFrontBlocked(FHitResult& OutHit) const
{
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(MoveDirection * 40.f, 0.f, 0.f);

	FVector HalfSize = FVector(20.f, 20.f, 50.f);
	FCollisionShape Box = FCollisionShape::MakeBox(HalfSize);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	return GetWorld()->SweepSingleByChannel(
		OutHit,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		Box,
		Params
	);
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
