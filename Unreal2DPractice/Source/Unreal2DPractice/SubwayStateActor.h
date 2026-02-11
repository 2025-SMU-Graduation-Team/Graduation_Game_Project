#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "SubwayStateActor.generated.h"

class AEndingDirector;

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

    virtual void Interact() override;

    void SetState(ESubwayState NewState);

protected:
    virtual void BeginPlay() override;

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
};
