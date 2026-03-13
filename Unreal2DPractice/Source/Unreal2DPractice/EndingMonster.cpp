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

	HandleObstacle(DeltaTime);

	float Distance = FVector::Dist(GetActorLocation(), EndLocation);

	CheckTurnPoint();
	CheckEndPoint();
}

void AEndingMonster::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnHitBoxOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (bHasKilledPlayer)
	{
		SetActorTickEnabled(false);
		return;
	}

	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player) return;

	bHasKilledPlayer = true;

	Player->PlayDeath();

	SetActorTickEnabled(false);
}

void AEndingMonster::CheckTurnPoint()
{
	const float DistanceX = FMath::Abs(GetActorLocation().X - TurnLocation.X);

	if (DistanceX < 30.f)
	{
		SetMoveDirection(-1.f * MoveDirection);
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
	if (!bBreakingObstacle)
	{
		if (IsFrontBlocked())
		{
			bBreakingObstacle = true;
			BreakTime = 0.f;

			SetMoveDirectionX(0.f);
			return;
		}
	}

	if (bBreakingObstacle)
	{
		BreakTime += DeltaTime;

		FVector Push = FVector(MoveDirection * BreakForce * DeltaTime, 0.f, 0.f);
		AddActorWorldOffset(Push, true);

		if (!IsFrontBlocked() || BreakTime >= BreakDuration)
		{
			bBreakingObstacle = false;
			BreakTime = 0.f;

			SetMoveDirectionX(MoveDirection);
		}

		return;
	}

	AddMovementInput(FVector(MoveDirection, 0.f, 0.f), MoveSpeed * DeltaTime);
}

bool AEndingMonster::IsFrontBlocked() const
{
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(MoveDirection * 40.f, 0.f, 0.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	return GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);
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
