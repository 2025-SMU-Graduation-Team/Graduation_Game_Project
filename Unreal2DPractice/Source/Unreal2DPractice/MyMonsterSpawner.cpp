
#include "MyMonsterSpawner.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"

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
}