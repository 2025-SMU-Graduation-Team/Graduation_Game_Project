#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraLimitVolume.generated.h"

class UBoxComponent;

UCLASS()
class UNREAL2DPRACTICE_API ACameraLimitVolume : public AActor
{
    GENERATED_BODY()

    public:
    ACameraLimitVolume();

    FBox GetLimitBounds() const;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnOverlapEnd(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

private:
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* Box;
};
