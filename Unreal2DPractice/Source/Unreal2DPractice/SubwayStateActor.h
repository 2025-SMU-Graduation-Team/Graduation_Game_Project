#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SubwayStateActor.generated.h"

class UBoxComponent;
class AEndingDirector;
class AMyPaperCharacter;

UENUM()
enum class ESubwayState : uint8
{
    Approaching,
    DoorsOpen,
    Leaving,
    Passed
};

UCLASS()
class UNREAL2DPRACTICE_API ASubwayStateActor : public AActor
{
    GENERATED_BODY()

    public:
    ASubwayStateActor();

    void Interact(AMyPaperCharacter* Player);

    void SetState(ESubwayState NewState);

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);

private:
    const TCHAR* StateToString(ESubwayState State);

private:
    UPROPERTY(EditInstanceOnly)
    AEndingDirector* EndingDirector;

    UPROPERTY(EditAnywhere, Category = "Subway")
    FVector NormalTeleportLocation;

    UPROPERTY(EditAnywhere, Category = "Subway")
    FVector HiddenTeleportLocation;

    ESubwayState CurrentState;

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerBox;
};
