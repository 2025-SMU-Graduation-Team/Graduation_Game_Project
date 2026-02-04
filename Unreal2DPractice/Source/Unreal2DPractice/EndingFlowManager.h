#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndingFlowManager.generated.h"

UENUM()
enum class EEndingType : uint8
{
    Normal,
    Hidden
};

UCLASS()
class UNREAL2DPRACTICE_API AEndingFlowManager : public AActor
{
    GENERATED_BODY()

    public:
    void RequestEnding(EEndingType Type);

private:
    UFUNCTION()
    void OnEndingLevelLoaded();

    EEndingType PendingEnding;

    FName PendingLevelName;
    FVector PendingTeleportLocation;
};
