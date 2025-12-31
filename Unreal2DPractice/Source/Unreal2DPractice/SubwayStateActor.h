#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "DelayedTaskData.h"
#include "SubwayStateActor.generated.h"

UENUM()
enum class ESubwayState : uint8
{
    Approaching,
    DoorsOpen,
    Leaving,
    Passed
};

UCLASS()
class UNREAL2DPRACTICE_API ASubwayStateActor : public AInteractableActor
{
    GENERATED_BODY()

public:
    ASubwayStateActor();

    void SetState(ESubwayState NewState);

    virtual void Interact() override;

protected:
    virtual void BeginPlay() override;

private:
    void HandleRide();
    void HandleSkip();

    ESubwayState CurrentState;

    const TCHAR* StateToString(ESubwayState State);

    UPROPERTY(EditAnywhere, Category = "Subway")
    FVector NormalTeleportLocation;

    UPROPERTY(EditAnywhere, Category = "Subway")
    FVector HiddenTeleportLocation;
};
