#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "LevelSequenceActor.h"
#include "EndingFlowManager.h"
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

    virtual void Interact() override;

    void SetState(ESubwayState NewState);

    UFUNCTION(BlueprintCallable)
    void HandleRide();

    UFUNCTION(BlueprintCallable)
    void HandleSkip();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditInstanceOnly)
    AEndingFlowManager* FlowManager;

private:
    void PlayEnterCutscene();

    const TCHAR* StateToString(ESubwayState State);

private:
    UPROPERTY(EditAnywhere, Category = "Cutscene")
    ALevelSequenceActor* DoorsOpenCutscene;

    UPROPERTY(EditAnywhere, Category = "Cutscene")
    ALevelSequenceActor* PassedCutscene;

    UPROPERTY(EditAnywhere, Category = "Subway")
    FVector NormalTeleportLocation;

    UPROPERTY(EditAnywhere, Category = "Subway")
    FVector HiddenTeleportLocation;

    UPROPERTY()
    ALevelSequenceActor* ActiveCutscene;

    ESubwayState CurrentState;
};
