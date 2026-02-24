// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPaperMonster.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "MyPaperCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"


// Sets default values
AMyPaperMonster::AMyPaperMonster()
{
 	PrimaryActorTick.bCanEverTick = true;

	Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	RootComponent = Flipbook;
	Flipbook->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetActorScale3D(FVector(2.5f, 2.5f, 2.5f));
	

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetBoxExtent(FVector(120.f, 30.f, 240.f));

	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionObjectType(ECC_WorldDynamic);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitBox->SetGenerateOverlapEvents(true);

	// DespawnVolume 감지용 센서
	DespawnSensor = CreateDefaultSubobject<UBoxComponent>(TEXT("DespawnSensor"));
	DespawnSensor->SetupAttachment(RootComponent);

	// 몬스터 몸 전체를 대충 커버하는 정도로
	DespawnSensor->SetBoxExtent(FVector(80.f, 40.f, 120.f));

	// Overlap 전용
	DespawnSensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DespawnSensor->SetCollisionObjectType(ECC_WorldDynamic);
	DespawnSensor->SetCollisionResponseToAllChannels(ECR_Ignore);

	// DespawnVolume이 WorldStatic이니까, 여기만 Overlap 열어줌
	DespawnSensor->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);

	DespawnSensor->SetGenerateOverlapEvents(true);

	WalkAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("WalkAudioComp"));
	WalkAudioComp->SetupAttachment(RootComponent);
	WalkAudioComp->bAutoActivate = false;
}


void AMyPaperMonster::BeginPlay()
{
	Super::BeginPlay();

	if (FB_Walk)
		SetState(EMonsterState::Walk);
	else
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
		HitBox->OnComponentBeginOverlap.AddDynamic(this, &AMyPaperMonster::OnHitBoxOverlap);
	}

	UE_LOG(LogTemp, Warning, TEXT("[Monster] FB_Idle=%s FB_Walk=%s FB_Attack=%s"),
		*GetNameSafe(FB_Idle), *GetNameSafe(FB_Walk), *GetNameSafe(FB_Attack));
}

void AMyPaperMonster::EnableDetection()
{
	bCanDetect = true;
}

void AMyPaperMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMovement(DeltaTime);
}

void AMyPaperMonster::UpdateMovement(float DeltaTime)
{
	// 공격 중엔 이동 안 함
	if (State == EMonsterState::Attack) return;
	
	// 방향 전환 없음: MoveDirX로만 직진
	AddActorWorldOffset(FVector(MoveDirX * MoveSpeed * DeltaTime, 0.f, 0.f), false);

	// 시각적으로 방향만 맞추기
	FVector Scale = GetActorScale3D();
	Scale.Y = 1.f;
	Scale.X = FMath::Abs(Scale.X) * MoveDirX;  // 오른쪽이면 +, 왼쪽이면 -
	SetActorScale3D(Scale);
}

void AMyPaperMonster::SetState(EMonsterState NewState)
{
	if (State == NewState) return;
	State = NewState;

	UPaperFlipbook* Desired = nullptr;

	switch (State)
	{
	case EMonsterState::Idle:
		Desired = FB_Idle;
		break;
	case EMonsterState::Walk:
		Desired = FB_Walk ? FB_Walk : FB_Idle;
		break;
	case EMonsterState::Attack:
		Desired = FB_Attack ? FB_Attack : (FB_Walk ? FB_Walk : FB_Idle);
		break;
	default:
		Desired = FB_Idle;
		break;
	}

	if (Desired && Flipbook->GetFlipbook() != Desired)
	{
		Flipbook->SetFlipbook(Desired);
	}
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

void AMyPaperMonster::StartWalkSound(USoundBase* InSound)
{
	if (!WalkAudioComp || !InSound) return;

	if (!WalkAudioComp->IsPlaying())
	{
		WalkAudioComp->SetSound(InSound);
		WalkAudioComp->Play();
	}
}

void AMyPaperMonster::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,
	bool bFromSweep,const FHitResult& SweepResult)
{
	if (bHasKilledPlayer) return;
	if (!bCanDetect) return;

	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player) return;

	UE_LOG(LogTemp, Warning, TEXT("[MonsterHit] bCanDetect=%d PlayerHidden=%d"), bCanDetect ? 1 : 0, Player->bIsHidden ? 1 : 0);

	if (Player->bIsHidden)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is hidden. Ignore."));
		return;
	}

	StartAttack(Player);
}

void AMyPaperMonster::StartAttack(AMyPaperCharacter* Player)
{
	if (!Player) return;
	if (bHasKilledPlayer) return;

	bHasKilledPlayer = true;
	Target = Player;

	// 더 이상 겹침/이동 안 하게
	bCanDetect = false;

	if (HitBox) HitBox->SetGenerateOverlapEvents(false);

	// 걷기 사운드 정지
	if (WalkAudioComp) WalkAudioComp->Stop();

	// 공격 사운드
	if (ShoutSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ShoutSound,
			GetActorLocation()
		);
	}

	// 공격 애니메이션 재생
	SetState(EMonsterState::Attack);

	// 공격 애니 길이만큼 대기
	float AttackDuration = 0.2f;
	if (FB_Attack)
	{
		AttackDuration = FB_Attack->GetTotalDuration();
	}

	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&AMyPaperMonster::ApplyAttackDamage,
		AttackDuration,
		false
	);
}

void AMyPaperMonster::ApplyAttackDamage()
{
	if (Target) Target->PlayDeath();
	Destroy();
}

