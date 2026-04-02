#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraShakeBase.h"
#include "Components/BoxComponent.h"
#include "EarthquakeEventActor.generated.h"

UCLASS()
class UNREAL2DPRACTICE_API AEarthquakeEventActor : public AActor
{
    GENERATED_BODY()

public:
    AEarthquakeEventActor();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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
    TArray<TSubclassOf<AActor>> BigRubbles;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    TArray<TSubclassOf<AActor>> SmallRubbles;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    float BigSpawnRatio = 0.65f;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    float BigDownImpulse = -1500.f;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    float SmallDownImpulse = -900.f;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    FVector RubbleSpawnPoint;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    int32 RubbleSpawnCount = 10;

    UPROPERTY(EditAnywhere, Category = "Rubble")
    float RubbleSpawnInterval = 0.2f;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBoxComponent> TriggerBox;

private:
    FTimerHandle PreviewShakeTimer;
    FTimerHandle CollapseStartTimer;
    FTimerHandle RubbleSpawnTimer;
    FTimerHandle StopShakeTimer;

    int32 SpawnedRubbleCount = 0;

    bool bEventStarted = false;
};
