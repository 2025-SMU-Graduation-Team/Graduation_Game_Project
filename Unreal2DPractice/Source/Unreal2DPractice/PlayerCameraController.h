#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraController.generated.h"

class USpringArmComponent;
class ACameraLimitVolume;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREAL2DPRACTICE_API UPlayerCameraController : public UActorComponent
{
    GENERATED_BODY()

    public:
    UPlayerCameraController();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    void SetLimitVolume(ACameraLimitVolume* NewVolume);

private:
    FVector GetClampedCameraTarget() const;

private:
    USpringArmComponent* SpringArm;
    ACameraLimitVolume* LimitVolume;
};
