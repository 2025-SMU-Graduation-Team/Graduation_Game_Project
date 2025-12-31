#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraShakeBase.h"
#include "EarthquakeEventActor.generated.h"

UCLASS()
class UNREAL2DPRACTICE_API AEarthquakeEventActor : public AActor
{
    GENERATED_BODY()

    public:
    AEarthquakeEventActor();

protected:
    virtual void BeginPlay() override;

private:
    void StartPreviewShake();
    void StartCollapse();
    void SpawnRubble();
    void StopShake();

private:
    UPROPERTY(EditAnywhere, Category = "Timing")
    float FirstShakeDelay = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Timing")
    float PreviewShakeDuration = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Timing")
    float CollapseDelayAfterPreview = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    TSubclassOf<UCameraShakeBase> CameraShake;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float ShakeScale = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    TSubclassOf<AActor> RubbleActor;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    FVector RubbleSpawnPoint;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    int32 RubbleSpawnCount = 10;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    float RubbleSpawnInterval = 0.2f;

private:
    FTimerHandle PreviewShakeTimer;
    FTimerHandle CollapseStartTimer;
    FTimerHandle RubbleSpawnTimer;
    FTimerHandle StopShakeTimer;

    int32 SpawnedRubbleCount = 0;
};
