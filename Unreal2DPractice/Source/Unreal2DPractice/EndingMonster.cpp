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

	bCanDetect = true;
	bHasKilledPlayer = false;

}

void AEndingMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bEndTriggered) return;

	float Distance = FVector::Dist(GetActorLocation(), EndLocation);

	if (Distance < 50.f)
	{
		bEndTriggered = true;

		if (Manager)
		{
			Manager->NotifyReachedEnd(this);
		}
	}
}

void AEndingMonster::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasKilledPlayer) return;

	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player) return;

	bHasKilledPlayer = true;

	Player->PlayDeath();

	SetActorTickEnabled(false);

	Destroy();
}

void AEndingMonster::SetMoveDirection(float Dir)
{
	SetMoveDirectionX(Dir);
}

void AEndingMonster::SetEndLocation(const FVector& InLocation)
{
	EndLocation = InLocation;
}

void AEndingMonster::SetManager(AEndingChaseManager* InManager)
{
	Manager = InManager;
}
