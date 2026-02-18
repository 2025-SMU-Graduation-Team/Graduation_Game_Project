// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "MyPaperMonster.generated.h"


class UPaperFlipbookComponent;
class UPaperFlipbook;
class AMyPaperCharacter;
class UBoxComponent;

UENUM(BlueprintType)
enum class EMonsterState : uint8 
{
    Idle,
    Walk,  
    Attack
};


UCLASS()
class UNREAL2DPRACTICE_API AMyPaperMonster : public APawn
{
	GENERATED_BODY()

public:
	AMyPaperMonster();

	virtual void Tick(float DeltaTime) override;

	void InitTarget(AMyPaperCharacter * InTarget, bool bUseDistance, float InRadius);
    void SetMoveDirectionX(float DirX);

    void StartWalkSound(USoundBase* WalkSound);

    UBoxComponent* GetHitBox() const { return HitBox; }

protected:
	virtual void BeginPlay() override;

    void UpdateMovement(float DeltaTime);
    void SetState(EMonsterState NewState);
    void EnableDetection();

    UFUNCTION()
    void OnHitBoxOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    void StartAttack(AMyPaperCharacter* Player);
    void ApplyAttackDamage();

    UPROPERTY(VisibleAnywhere, Category = "Monster|Visual")
    UPaperFlipbookComponent* Flipbook;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Anim")
    UPaperFlipbook* FB_Idle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Anim")
    UPaperFlipbook* FB_Walk;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Anim")
    UPaperFlipbook* FB_Attack;

    UPROPERTY(VisibleAnywhere, Category = "Monster|Collision")
    UBoxComponent* HitBox;

    UPROPERTY(VisibleAnywhere, Category = "Monster|Collision")
    UBoxComponent* DespawnSensor;

    UPROPERTY(EditAnywhere, Category = "AI")
    float MoveSpeed = 250.f;

    UPROPERTY(EditAnywhere, Category = "Detect")
    bool bDetectByDistance = true;

    UPROPERTY(EditAnywhere, Category = "Detect", meta = (EditCondition = "bDetectByDistance"))
    float DetectRadius = 80.f;

    bool bCanDetect = false;
    FTimerHandle DetectionDelayHandle;

    FTimerHandle AttackTimerHandle;

    UPROPERTY()
    AMyPaperCharacter* Target = nullptr;

    EMonsterState State = EMonsterState::Idle;

    float MoveDirX = 1.f; // +1 = 오른쪽으로 직진, -1 = 왼쪽으로 직진
private:
    UPROPERTY(VisibleAnywhere, Category = "Sound")
    UAudioComponent* WalkAudioComp;

    UPROPERTY(EditAnywhere, Category = "Sound")
    USoundBase* WalkSound;

    UPROPERTY(EditAnywhere, Category = "Sound")
    USoundBase* ShoutSound;

    bool bHasKilledPlayer = false;
};