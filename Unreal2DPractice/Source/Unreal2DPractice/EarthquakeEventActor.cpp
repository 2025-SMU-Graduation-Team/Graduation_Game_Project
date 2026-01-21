#include "EarthquakeEventActor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

AEarthquakeEventActor::AEarthquakeEventActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEarthquakeEventActor::BeginPlay()
{
    Super::BeginPlay();

    GetWorldTimerManager().SetTimer(
        PreviewShakeTimer,
        this,
        &AEarthquakeEventActor::StartPreviewShake,
        FirstShakeDelay,
        false
    );
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
    if (!RubbleActor)
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

    GetWorld()->SpawnActor<AActor>(
        RubbleActor,
        RubbleSpawnPoint,
        FRotator::ZeroRotator
    );

    SpawnedRubbleCount++;
}

void AEarthquakeEventActor::StopShake()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->ClientStopCameraShake(CameraShake, false);
    }
}
