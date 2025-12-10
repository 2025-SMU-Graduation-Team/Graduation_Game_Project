
#include "MyMonsterSpawner.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"          // ★ 추가: TActorIterator 사용

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
            this, &AMyMonsterSpawner::OnTriggerBegin);
    }

    // 플레이어 캐싱
    CachedPlayer = Cast<AMyPaperCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void AMyMonsterSpawner::OnTriggerBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("OnTriggerBegin: Something overlapped! OtherActor = %s"),
        *GetNameSafe(OtherActor));

    // 이미 한 번 스폰했다면, 소리/타이머/스폰 전부 막기
    if (bOneShot && bHasSpawned)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnTriggerBegin: Already spawned once, ignore"));
        return;
    }

    if (!OtherActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnTriggerBegin: OtherActor is null"));
        return;
    }

    // 아직 CachedPlayer가 비어 있으면, 이번에 들어온 액터를 플레이어로 캐싱
    if (!CachedPlayer)
    {
        CachedPlayer = Cast<AMyPaperCharacter>(OtherActor);
        UE_LOG(LogTemp, Warning, TEXT("OnTriggerBegin: CachedPlayer set to %s"),
            *GetNameSafe(CachedPlayer));
    }

    // (원래 의도대로라면 여기서 타입 체크)
    if (OtherActor != CachedPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnTriggerBegin: OtherActor is not CachedPlayer"));
        return;
    }

    bHasSpawned = true;

    // 랜덤 방향 결정
    bSpawnFromLeft = FMath::RandBool();

    UE_LOG(LogTemp, Warning, TEXT("OnTriggerBegin: Will spawn from %s"),
        bSpawnFromLeft ? TEXT("Left") : TEXT("Right"));

    USceneComponent* SpawnPoint = bSpawnFromLeft ? LeftSpawnPoint : RightSpawnPoint;

    // 좌/우 사운드 재생
    if (SpawnSound && SpawnPoint)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            SpawnSound,
            SpawnPoint->GetComponentLocation());
        UE_LOG(LogTemp, Warning, TEXT("OnTriggerBegin: Played sound"));
    }

    if (SpawnDelay > 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnTriggerBegin: Set timer for spawn (%.2f)"), SpawnDelay);

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
}

void AMyMonsterSpawner::SpawnMonster()
{
    UE_LOG(LogTemp, Warning, TEXT("SpawnMonster() called"));

    if (!MonsterClass)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnMonster aborted: MonsterClass is NULL"));
        return;
    }

    if (!CachedPlayer)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnMonster aborted: CachedPlayer is NULL"));
        return;
    }

    // 월드에 이미 몬스터가 있는지 체크 (전역 1마리 보장)
    for (TActorIterator<AMyPaperMonster> It(GetWorld()); It; ++It)
    {
        AMyPaperMonster* Existing = *It;
        if (IsValid(Existing))
        {
            UE_LOG(LogTemp, Warning,
                TEXT("SpawnMonster: Monster already exists (%s), skip spawning"),
                *Existing->GetName());
            return;    // 이미 월드에 살아 있는 몬스터가 1마리 있음 → 스폰 중단
        }
    }

    USceneComponent* SpawnPoint = bSpawnFromLeft ? LeftSpawnPoint : RightSpawnPoint;
    if (!SpawnPoint)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnMonster aborted: SpawnPoint is NULL"));
        return;
    }

    const FVector Loc = SpawnPoint->GetComponentLocation();
    const FRotator Rot = FRotator::ZeroRotator;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AMyPaperMonster* Monster = GetWorld()->SpawnActor<AMyPaperMonster>(
        MonsterClass, Loc, Rot, Params);

    if (Monster)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnMonster: Monster spawned at %s"), *Loc.ToString());
        Monster->InitTarget(CachedPlayer, true, 800.f);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnMonster: Failed to spawn monster"));
    }
}