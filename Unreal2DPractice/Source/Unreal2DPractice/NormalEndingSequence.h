#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NormalEndingSequence.generated.h"

class AMyPaperCharacter;
class UNormalEndingWidget;
class UPaperFlipbook;

UCLASS()
class UNREAL2DPRACTICE_API ANormalEndingSequence : public AActor
{
    GENERATED_BODY()

    public:
    ANormalEndingSequence();

    void StartSequence(AMyPaperCharacter* Player, FVector TeleportLocation);

protected:
    virtual void BeginPlay() override;

private:
    void StartMove();
    void UpdateMove();
    void OnMoveFinished();
    void ShowEndingWidget();
    void FinishSequence();

private:
    UPROPERTY(EditAnywhere)
    float MoveDuration = 1.0f;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UNormalEndingWidget> EndingWidgetClass;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UPaperFlipbook> BackFlipbook;

    UPROPERTY(EditDefaultsOnly, Category = "Ending")
    FName DestinationLevelName = TEXT("Normal");

private:
    TObjectPtr<AMyPaperCharacter> CachedPlayer;
    TObjectPtr<UNormalEndingWidget> ActiveWidget;

    FVector PendingTeleportLocation;
    FVector StartLocation;
    FVector TargetLocation;

    float MoveElapsed = 0.f;

    FTimerHandle MoveTimer;
};
