// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPaperMonster.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "MyPaperCharacter.h"
#include "Components/BoxComponent.h"
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

	// Idle Anim
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleFB(
		TEXT("/Game/Monster/MonsterIdle.MonsterIdle"));
	if (IdleFB.Succeeded())
	{
		FB_Idle = IdleFB.Object;
		Flipbook->SetFlipbook(FB_Idle);
	}

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);

	HitBox->SetBoxExtent(FVector(40.f, 10.f, 80.f));

	// Collision Settnig: Sense the Player Pawn
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionObjectType(ECC_WorldDynamic);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitBox->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AMyPaperMonster::BeginPlay()
{
	Super::BeginPlay();
	SetState(EMonsterState::Idle);

	// 0.5초 후 감지 활성화
	GetWorld()->GetTimerManager().SetTimer(
		DetectionDelayHandle,
		this,
		&AMyPaperMonster::EnableDetection,
		0.5f,
		false
	);
	if (HitBox)
	{
		HitBox->OnComponentBeginOverlap.AddDynamic(
			this,
			&AMyPaperMonster::OnHitBoxOverlap
		);
	}
}

void AMyPaperMonster::EnableDetection()
{
	bCanDetect = true;
}

void AMyPaperMonster::InitTarget(AMyPaperCharacter* InTarget, bool bUseDistance, float InRadius)
{
	Target = InTarget;
	bDetectByDistance = bUseDistance;
	DetectRadius = InRadius;
}

void AMyPaperMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Target) return;

	UpdateMovement(DeltaTime);
}

void AMyPaperMonster::UpdateMovement(float DeltaTime)
{
	const FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	const FVector Dir = ToTarget.GetSafeNormal2D();

	// Left/Right Face
	FaceToTarget();

	// Move(State is always Idle, No walking anim for now)
	AddActorWorldOffset(Dir * MoveSpeed * DeltaTime, false);
}

void AMyPaperMonster::FaceToTarget()
{
	const float Sign = (Target->GetActorLocation().X - GetActorLocation().X) >= 0 ? 1.f : -1.f;
	FVector Scale = GetActorScale3D();
	Scale.Y = 1.f;
	Scale.X = FMath::Abs(Scale.X) * Sign;
	SetActorScale3D(Scale);
}

void AMyPaperMonster::SetState(EMonsterState NewState)
{
	if (State == NewState)
	{
		return;
	}

	State = NewState;

	if (FB_Idle)
	{
		Flipbook->SetFlipbook(FB_Idle);
	}
}

void AMyPaperMonster::OnHitBoxOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!bCanDetect)
	{
		return; // Just after Spawn, Off the sensing for moment
	}

	if (!OtherActor)
	{
		return;
	}

	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player)
	{
		return; //if not player, ignore
	}

	// 1.Player Die
	Player->PlayDeath();

	// 2.Monster Move/Detection Stop
	bCanDetect = false;
	SetActorTickEnabled(false);
	if (HitBox)
	{
		HitBox->SetGenerateOverlapEvents(false);
	}

	// 3.Eliminate Actor
	Destroy();
}

