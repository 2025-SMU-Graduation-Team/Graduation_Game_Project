#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NormalEndingSequence.generated.h"

class AMyPaperCharacter;
class USubLevelTaskManager;

UENUM()
enum class ENormalEndingState : uint8
{
    None,
    Moving,
    ClosingDoor,
    ShowingImage,
    FadingOut
};

UCLASS()
class UNREAL2DPRACTICE_API ANormalEndingSequence : public AActor
{
    GENERATED_BODY()

    public:
    ANormalEndingSequence();

    virtual void Tick(float DeltaTime) override;

    void StartSequence(AMyPaperCharacter* Player, FVector TeleportLocation);

protected:
    virtual void BeginPlay() override;

private:
    void UpdateMove(float DeltaTime);
    void CloseDoor();
    void StartImagePhase();
    void UpdateCameraShake(float DeltaTime);
    void FinishSequence();

    ENormalEndingState State;

    AMyPaperCharacter* CachedPlayer;
    FVector StartLocation;
    FVector TargetLocation;
    FVector PendingTeleportLocation;

    float MoveSpeed;
    float ShakeTime;
    float ShakeDuration;
    float ShakeAmplitude;
    float ShakeSpeed;

    UPROPERTY(EditAnywhere)
    TWeakObjectPtr<AActor> ScreenDoorActor;

    UPROPERTY(EditAnywhere)
    TWeakObjectPtr<AActor> SubwayDoorActor;

    UPROPERTY(EditAnywhere)
    USubLevelTaskManager* TaskManager;

    UPROPERTY(EditAnywhere)
    UTexture2D* EndingImage;
};
