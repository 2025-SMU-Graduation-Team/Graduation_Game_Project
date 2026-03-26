#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraController.generated.h"

class USpringArmComponent;
class ACameraLimitVolume;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREAL2DPRACTICE_API UPlayerCameraController : public UActorComponent
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    UPlayerCameraController();
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    void SetLimitVolume(ACameraLimitVolume* NewVolume);
    void ClearLimitVolume(ACameraLimitVolume* VolumeToClear, bool bForceClear = false);
    void RefreshCameraPosition();

    UPROPERTY()
    UCameraComponent* Camera;

    UPROPERTY(EditAnywhere, Category = "Camera")
    FVector cameraOffset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Camera")
    bool bKeepLastLimitWhenExited = true;
private:
    FVector GetClampedCameraTarget() const;
    float GetHalfViewHeight() const;

private:
    USpringArmComponent* SpringArm;
    ACameraLimitVolume* LimitVolume;
};
