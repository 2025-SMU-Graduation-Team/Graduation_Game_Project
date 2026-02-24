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

	float Distance = FVector::Dist(GetActorLocation(), EndLocation);

	CheckTurnPoint();
	CheckEndPoint();
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
