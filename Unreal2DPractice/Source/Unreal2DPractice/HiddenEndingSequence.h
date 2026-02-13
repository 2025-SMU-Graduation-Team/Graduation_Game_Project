#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HiddenEndingSequence.generated.h"

class AMyPaperCharacter;

UENUM()
enum class EHiddenEndingState : uint8
{
    None,
    WaitingInput,
    Moving,
    FadingOut
};

UCLASS()
class UNREAL2DPRACTICE_API AHiddenEndingSequence : public AActor
{
    GENERATED_BODY()

    public:
    AHiddenEndingSequence();

    void StartSequence(AMyPaperCharacter* Player, FVector TeleportLocation);

protected:
    virtual void BeginPlay() override;

private:
    void StartMove();
    void UpdateMove();
    void FinishSequence();

    EHiddenEndingState State;

    AMyPaperCharacter* CachedPlayer;
    FVector StartLocation;
    FVector TargetLocation;
    FVector PendingTeleportLocation;

    float MoveSpeed;

    UPROPERTY(EditAnywhere)
    UAnimMontage* BackMontage;

    float MoveElapsed = 0.f;
    float MoveDuration = 2.5f;

    FTimerHandle MoveTimer;
};
