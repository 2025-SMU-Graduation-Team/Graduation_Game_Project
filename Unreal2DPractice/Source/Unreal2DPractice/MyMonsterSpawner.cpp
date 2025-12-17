
#include "MyMonsterSpawner.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

#include "MyPaperMonster.h"
#include "MyPaperCharacter.h"

AMyMonsterSpawner::AMyMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    // 루트
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // 트리거 박스
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));

    // 왼쪽 / 오른쪽 스폰 포인트
    LeftSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeftSpawnPoint"));
    LeftSpawnPoint->SetupAttachment(RootComponent);
    LeftSpawnPoint->SetRelativeLocation(FVector(-300.f, 0.f, 0.f));

    RightSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RightSpawnPoint"));
    RightSpawnPoint->SetupAttachment(RootComponent);
    RightSpawnPoint->SetRelativeLocation(FVector(300.f, 0.f, 0.f));
}

void AMyMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();

    // 트리거 오버랩 이벤트 바인딩
    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(
            this,
            &AMyMonsterSpawner::OnTriggerBegin);
    }

    // 플레이어 캐싱
    CachedPlayer = Cast<AMyPaperCharacter>(
        UGameplayStatics::GetPlayerPawn(this, 0));
}

void AMyMonsterSpawner::OnTriggerBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // 이미 한 번 썼고 OneShot이면 무시
    if (bOneShot && bHasSpawned)
    {
        return;
    }

    if (!OtherActor || OtherActor != CachedPlayer)
    {
        return;
    }

    bHasSpawned = true;

    // 랜덤 방향 결정
    bSpawnFromLeft = FMath::RandBool();

    USceneComponent* SpawnPoint = bSpawnFromLeft ? LeftSpawnPoint : RightSpawnPoint;
    USoundBase* UseSound = bSpawnFromLeft ? Sound_Left : Sound_Right;

    // 좌/우 사운드 재생
    if (UseSound && SpawnPoint)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            UseSound,
            SpawnPoint->GetComponentLocation());
    }

    // 1.5초(또는 설정된 시간) 후 몬스터 스폰
    if (SpawnDelay > 0.f)
    {
        GetWorldTimerManager().SetTimer(
            TimerHandle_Spawn,
            this,
            &AMyMonsterSpawner::SpawnMonster,
            SpawnDelay,
            false);
    }
    else
    {
        SpawnMonster();
    }

    // 다시 안 쓰게 하고 싶으면 트리거 끄기
    if (bOneShot && TriggerBox)
    {
        TriggerBox->SetGenerateOverlapEvents(false);
    }
}

void AMyMonsterSpawner::SpawnMonster()
{
    if (!MonsterClass || !CachedPlayer)
    {
        return;
    }

    USceneComponent* SpawnPoint = bSpawnFromLeft ? LeftSpawnPoint : RightSpawnPoint;
    if (!SpawnPoint)
    {
        return;
    }

    const FVector Loc = SpawnPoint->GetComponentLocation();
    const FRotator Rot = FRotator::ZeroRotator;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AMyPaperMonster* Monster = GetWorld()->SpawnActor<AMyPaperMonster>(
        MonsterClass,
        Loc,
        Rot,
        Params);

    if (Monster)
    {
        // 몬스터에게 타겟과 감지 거리 넘겨줌
        Monster->InitTarget(CachedPlayer, true, 800.f);
    }
}
