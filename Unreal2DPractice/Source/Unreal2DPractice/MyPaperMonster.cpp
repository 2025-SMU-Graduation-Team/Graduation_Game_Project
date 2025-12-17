// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPaperMonster.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "MyPaperCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMyPaperMonster::AMyPaperMonster()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	RootComponent = Flipbook;
	Flipbook->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AMyPaperMonster::BeginPlay()
{
	Super::BeginPlay();
	SetState(EMonsterState::Idle);
}


void AMyPaperMonster::InitTarget(AMyPaperCharacter* InTarget, bool bUseDistance, float InRadius)
{
	Target = InTarget;
	bDetectByDistance = bUseDistance;
	DetectRadius = InRadius;
}

// Called every frame
void AMyPaperMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Target) return;

	UpdateMovement(DeltaTime);
	UpdateDetection();
}

void AMyPaperMonster::UpdateMovement(float DeltaTime)
{
	const FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	const FVector Dir = ToTarget.GetSafeNormal2D(); // 2D 기준

	// 좌/우 바라보기
	FaceToTarget();

	// 이동
	AddActorWorldOffset(Dir * MoveSpeed * DeltaTime, false);

	if (State != EMonsterState::Walk) SetState(EMonsterState::Walk);
}

void AMyPaperMonster::FaceToTarget()
{
	const float Sign = (Target->GetActorLocation().X - GetActorLocation().X) >= 0 ? 1.f : -1.f;
	FVector Scale = GetActorScale3D();
	Scale.Y = 1.f;
	Scale.X = FMath::Abs(Scale.X) * Sign;
	SetActorScale3D(Scale);
}

void AMyPaperMonster::UpdateDetection()
{
	const float Dist = FVector::Dist2D(GetActorLocation(), Target->GetActorLocation());

	bool bDetected = false;
	if (bDetectByDistance)
	{
		bDetected = (Dist <= DetectRadius);
	}
	else
	{
		FHitResult Hit;
		FVector S = GetActorLocation();
		FVector E = Target->GetActorLocation();
		FCollisionQueryParams P(NAME_None, false, this);
		bDetected = (GetWorld()->LineTraceSingleByChannel(Hit, S, E, LOSChannel, P) && Hit.GetActor() == Target && Dist <= DetectRadius);
	}

	if (bDetected)
	{
		SetState(EMonsterState::Attack);
		//if (Target) Target->PlayerDeath(); // TODO: 플레이어 쪽 구현 후 활성화
	}
}

void AMyPaperMonster::SetState(EMonsterState NewState)
{
    State = NewState;
    switch (State)
    {
    case EMonsterState::Idle:
        if (FB_Idle) Flipbook->SetFlipbook(FB_Idle);
        break;
    case EMonsterState::Walk:
        if (FB_Walk) Flipbook->SetFlipbook(FB_Walk);
        break;
    case EMonsterState::Attack:
        if (FB_Attack) Flipbook->SetFlipbook(FB_Attack);
        break;
    }
}
//// Called to bind functionality to input
//void AMyPaperMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

