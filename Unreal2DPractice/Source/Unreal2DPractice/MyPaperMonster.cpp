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

	SetActorScale3D(FVector(2.5f, 2.5f, 2.5f));

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

	HitBox->SetBoxExtent(FVector(120.f, 30.f, 240.f));

	// Collision Settnig: Sense the Player Pawn
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionObjectType(ECC_WorldDynamic);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); //added
	HitBox->SetGenerateOverlapEvents(true);

	//***추가: DespawnVolume 감지용 센서
	DespawnSensor = CreateDefaultSubobject<UBoxComponent>(TEXT("DespawnSensor"));
	DespawnSensor->SetupAttachment(RootComponent);

	// 몬스터 몸 전체를 대충 커버하는 정도로
	DespawnSensor->SetBoxExtent(FVector(80.f, 40.f, 120.f));

	// Overlap 전용
	DespawnSensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DespawnSensor->SetCollisionObjectType(ECC_Pawn); // 혹은 WorldDynamic 아무거나 OK
	DespawnSensor->SetCollisionResponseToAllChannels(ECR_Ignore);

	// DespawnVolume이 WorldStatic이니까, 여기만 Overlap 열어줌
	DespawnSensor->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);

	DespawnSensor->SetGenerateOverlapEvents(true);
	//***여기까지

	WalkAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("WalkAudioComp"));
	WalkAudioComp->SetupAttachment(RootComponent);
	WalkAudioComp->bAutoActivate = false;
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
void AMyPaperMonster::StartWalkSound(USoundBase* InSound)
{
	if (!WalkAudioComp || !InSound) return;

	if (!WalkAudioComp->IsPlaying())
	{
		WalkAudioComp->SetSound(InSound);
		WalkAudioComp->Play();
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

void AMyPaperMonster::SetMoveDirectionX(float DirX)
{
	MoveDirX = (DirX >= 0.f) ? 1.f : -1.f;
	UE_LOG(LogTemp, Warning, TEXT("[Monster] SetMoveDirectionX = %.0f"), MoveDirX);
}

void AMyPaperMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMovement(DeltaTime);
}

void AMyPaperMonster::UpdateMovement(float DeltaTime)
{
	// 방향 전환 없음: MoveDirX로만 직진
	AddActorWorldOffset(FVector(MoveDirX * MoveSpeed * DeltaTime, 0.f, 0.f), false);

	// 시각적으로 방향만 맞추기
	FVector Scale = GetActorScale3D();
	Scale.Y = 1.f;
	Scale.X = FMath::Abs(Scale.X) * MoveDirX;  // 오른쪽이면 +, 왼쪽이면 -
	SetActorScale3D(Scale);
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
	if (bHasKilledPlayer) return;

	if (!bCanDetect)
	{
		return; // Just after Spawn, Off the sensing for moment
	}

	if (!OtherActor)
	{
		return;
	}

	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player) return;

	UE_LOG(LogTemp, Warning, TEXT("[MonsterHit] bCanDetect=%d PlayerHidden=%d"),
		bCanDetect ? 1 : 0,
		Player->bIsHidden ? 1 : 0);

	if (Player->bIsHidden)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is hidden. Ignore."));
		return;
	}

	bHasKilledPlayer = true;


	if (WalkAudioComp)
	{
		WalkAudioComp->Stop();
	}

	if (ShoutSound) 
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ShoutSound,
			GetActorLocation()
		);
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

