#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndingFlowManager.generated.h"

UENUM(BlueprintType)
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
    AEndingFlowManager();

    UFUNCTION(BlueprintCallable)
    void RequestEnding(EEndingType Type, const FVector& TeleportLocation);

protected:
    virtual void BeginPlay() override;

private:
    void LoadEndingLevel();
    UFUNCTION()
    void OnEndingLevelLoaded();
    void MovePlayer();
    void UnloadCurrentSubLevel();

private:
    FName CurrentSubLevelName;
    FName PendingLevelName;
    FVector PendingTeleportLocation;
};
