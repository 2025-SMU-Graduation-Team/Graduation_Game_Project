#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HiddenEndingReturnDispatcher.generated.h"

UENUM(BlueprintType)
enum class EHiddenEndingReturnTriggerMode : uint8
{
    ImmediateReturn,
    FinalTargetLevel
};

UCLASS()
class UNREAL2DPRACTICE_API AHiddenEndingReturnDispatcher : public AActor
{
    GENERATED_BODY()

public:
    AHiddenEndingReturnDispatcher();

protected:
    virtual void BeginPlay() override;

private:
    bool ShouldDispatch() const;
    void ActivateManagedActors();
    void NotifyManagedActors();

private:
    UPROPERTY(EditAnywhere, Category = "HiddenEnding")
    EHiddenEndingReturnTriggerMode TriggerMode = EHiddenEndingReturnTriggerMode::ImmediateReturn;

    UPROPERTY(EditAnywhere, Category = "HiddenEnding", meta = (EditCondition = "TriggerMode == EHiddenEndingReturnTriggerMode::FinalTargetLevel", EditConditionHides))
    FName TargetLevelName;

    UPROPERTY(EditInstanceOnly, Category = "HiddenEnding")
    TArray<TObjectPtr<AActor>> ActorsToActivate;

    UPROPERTY(EditInstanceOnly, Category = "HiddenEnding")
    TArray<TObjectPtr<AActor>> ActorsToNotify;

    UPROPERTY(EditAnywhere, Category = "HiddenEnding")
    bool bEnableCollisionOnActivate = true;

    UPROPERTY(EditAnywhere, Category = "HiddenEnding")
    bool bEnableTickOnActivate = true;
};
