// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingMonster.h"
#include "MyPaperCharacter.h"
#include "EndingChaseManager.h"
#include "GameFramework/PawnMovementComponent.h"

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
	AActor::Tick(DeltaTime);

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

void AEndingMonster::StopMonster()
{
	if (UPawnMovementComponent* MoveComp = GetMovementComponent())
	{
		MoveComp->StopMovementImmediately();
	}
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
	if (ObstacleState == EObstacleState::None)
	{
		FHitResult Hit;
		if (IsFrontBlocked(Hit))
		{
			AActor* HitActor = Hit.GetActor();

			UE_LOG(LogTemp, Warning, TEXT("Blocked - Actor: %s"),
				HitActor ? *HitActor->GetName() : TEXT("None"));

			if (HitActor && HitActor->ActorHasTag(TEXT("Breakable")))
			{
				CurrentObstacle = HitActor;
				ObstacleState = EObstacleState::WaitingBeforeBreak;
				ObstacleTimer = 0.f;

				SetState(EMonsterState::Idle);
				UE_LOG(LogTemp, Warning, TEXT("Wait before break: %s"), *HitActor->GetName());
				return;
			}
		}

		SetState(EMonsterState::Walk);
		AddActorWorldOffset(FVector(MoveDirection * MoveSpeed * DeltaTime, 0.f, 0.f), false);

		FVector Scale = GetActorScale3D();
		Scale.Y = 1.f;
		Scale.X = FMath::Abs(Scale.X) * MoveDirection;
		SetActorScale3D(Scale);
		return;
	}

	ObstacleTimer += DeltaTime;

	if (ObstacleState == EObstacleState::WaitingBeforeBreak)
	{
		SetState(EMonsterState::Idle);
		ObstacleTimer += DeltaTime;

		if (ObstacleTimer >= WaitBeforeBreak)
		{
			ObstacleState = EObstacleState::Breaking;
			ObstacleTimer = 0.f;
			UE_LOG(LogTemp, Warning, TEXT("Start breaking"));
		}
		return;
	}

	if (ObstacleState == EObstacleState::Breaking)
	{
		SetState(EMonsterState::Idle);
		ObstacleTimer += DeltaTime;

		if (ObstacleTimer >= BreakDuration)
		{
			if (CurrentObstacle && IsValid(CurrentObstacle))
			{
				const bool bDestroyed = CurrentObstacle->Destroy();
				UE_LOG(LogTemp, Warning, TEXT("Destroy result: %s"),
					bDestroyed ? TEXT("true") : TEXT("false"));
			}

			ObstacleState = EObstacleState::WaitingAfterBreak;
			ObstacleTimer = 0.f;
			UE_LOG(LogTemp, Warning, TEXT("Wait after break"));
		}
		return;
	}

	if (ObstacleState == EObstacleState::WaitingAfterBreak)
	{
		SetState(EMonsterState::Idle);
		ObstacleTimer += DeltaTime;

		if (ObstacleTimer >= WaitAfterBreak)
		{
			CurrentObstacle = nullptr;
			ObstacleState = EObstacleState::None;
			ObstacleTimer = 0.f;
			UE_LOG(LogTemp, Warning, TEXT("Resume move"));
		}
		return;
	}
}

bool AEndingMonster::IsFrontBlocked(FHitResult& OutHit) const
{
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(MoveDirection * 80.f, 0.f, 0.f);

	const FVector HalfSize(30.f, 20.f, 50.f);
	const FCollisionShape Box = FCollisionShape::MakeBox(HalfSize);

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	TArray<FHitResult> Hits;
	const bool bHit = GetWorld()->SweepMultiByObjectType(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ObjParams,
		Box,
		Params
	);

	if (!bHit)
	{
		return false;
	}

	for (const FHitResult& Hit : Hits)
	{
		UPrimitiveComponent* HitComp = Hit.GetComponent();
		if (HitComp && HitComp->ComponentHasTag(TEXT("Breakable")))
		{
			OutHit = Hit;
			return true;
		}
	}

	return false;
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
