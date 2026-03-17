#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpeningDoorInterface.h"
#include "ConfigurableDoorActor.generated.h"

class USceneComponent;

UCLASS()
class UNREAL2DPRACTICE_API AConfigurableDoorActor : public AActor, public IOpeningDoorInterface
{
    GENERATED_BODY()

public:
    AConfigurableDoorActor();

    virtual void Tick(float DeltaTime) override;

    virtual void OpenDoor_Implementation() override;
    virtual void CloseDoor_Implementation() override;
    virtual void RestoreOpenedState_Implementation() override;

protected:
    virtual void BeginPlay() override;

private:
    USceneComponent* FindDoorComponentByName(const FName& ComponentName) const;
    void CacheDoorComponents();
    void ApplyClosedTransform();
    void ApplyOpenTransform();
    void StartTransition();
    void FinishTransition(float FinalAlpha);
    void ApplyAlpha(float Alpha);
    FVector LerpVector(const FVector& Start, const FVector& End, float Alpha) const;

private:
    UPROPERTY(VisibleAnywhere, Category = "Door")
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(EditAnywhere, Category = "Door")
    FName LeftDoorComponentName = TEXT("Left Door");

    UPROPERTY(EditAnywhere, Category = "Door")
    FName RightDoorComponentName = TEXT("Right Door");

    UPROPERTY(EditAnywhere, Category = "Door")
    FVector LeftDoorClosedRelativeLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Door")
    FVector LeftDoorOpenRelativeLocation = FVector(-46.666666f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Door")
    FVector RightDoorClosedRelativeLocation = FVector(23.5f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Door")
    FVector RightDoorOpenRelativeLocation = FVector(46.666666f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Door", meta = (ClampMin = "0.0"))
    float TransitionDuration = 0.35f;

    UPROPERTY(EditAnywhere, Category = "Door")
    bool bStartOpened = false;

    UPROPERTY(EditAnywhere, Category = "Door")
    bool bUseSmoothTransition = true;

    UPROPERTY(EditAnywhere, Category = "Door")
    float InterpExponent = 2.f;

    UPROPERTY(Transient)
    TObjectPtr<USceneComponent> LeftDoorComponent;

    UPROPERTY(Transient)
    TObjectPtr<USceneComponent> RightDoorComponent;

    bool bIsTransitioning = false;
    float TransitionElapsed = 0.f;
    float TransitionStartAlpha = 0.f;
    float TransitionTargetAlpha = 0.f;
    float CurrentAlpha = 0.f;
};
