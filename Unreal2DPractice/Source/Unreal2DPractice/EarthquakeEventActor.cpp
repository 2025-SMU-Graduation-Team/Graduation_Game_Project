#include "EarthquakeEventActor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

AEarthquakeEventActor::AEarthquakeEventActor()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AEarthquakeEventActor::BeginPlay()
{
    Super::BeginPlay();

    if (!TriggerBox)
    {
        UE_LOG(LogTemp, Error, TEXT("TriggerBox is null"));
        return;
    }

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AEarthquakeEventActor::OnTriggerBeginOverlap);
}

void AEarthquakeEventActor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bEventStarted)
        return;

    if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
    {
        bEventStarted = true;

        GetWorldTimerManager().SetTimer(PreviewShakeTimer, this, &AEarthquakeEventActor::StartPreviewShake,
            FirstShakeDelay, false);
    }
}

void AEarthquakeEventActor::StartPreviewShake()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->ClientStartCameraShake(CameraShake, ShakeScale);
    }

    GetWorldTimerManager().SetTimer(
        StopShakeTimer,
        this,
        &AEarthquakeEventActor::StopShake,
        PreviewShakeDuration,
        false
    );

    GetWorldTimerManager().SetTimer(
        CollapseStartTimer,
        this,
        &AEarthquakeEventActor::StartCollapse,
        CollapseDelayAfterPreview,
        false
    );
}

void AEarthquakeEventActor::StartCollapse()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->ClientStartCameraShake(CameraShake, ShakeScale);
    }

    SpawnedRubbleCount = 0;

    GetWorldTimerManager().SetTimer(
        RubbleSpawnTimer,
        this,
        &AEarthquakeEventActor::SpawnRubble,
        RubbleSpawnInterval,
        true
    );
}

void AEarthquakeEventActor::SpawnRubble()
{
    if (RubbleActors.Num() == 0)
    {
        StopShake();
        return;
    }

    if (SpawnedRubbleCount >= RubbleSpawnCount)
    {
        GetWorldTimerManager().ClearTimer(RubbleSpawnTimer);
        StopShake();
        return;
    }

    int32 Index = FMath::RandRange(0, RubbleActors.Num() - 1);
    TSubclassOf<AActor> SelectedClass = RubbleActors[Index];

    if (SelectedClass)
    {
        GetWorld()->SpawnActor<AActor>(
            SelectedClass,
            RubbleSpawnPoint,
            FRotator::ZeroRotator
        );
    }

    SpawnedRubbleCount++;
}

void AEarthquakeEventActor::StopShake()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->ClientStopCameraShake(CameraShake, false);
    }
}
