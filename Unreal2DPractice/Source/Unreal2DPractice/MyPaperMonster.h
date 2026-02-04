// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

#include "MyPaperMonster.generated.h"


class UPaperFlipbookComponent;
class UPaperFlipbook;
class AMyPaperCharacter; //class APlayer2DCharacter;
class UBoxComponent;

UENUM(BlueprintType)
enum class EMonsterState : uint8 
{
    Idle,       // 대기/순찰
    Chase,      // 추격
    Search,     // 수색 (놓친 뒤 주변 탐색)
    Cooldown    // 잠깐 쉬었다가 Idle 복귀
};


UCLASS()
class UNREAL2DPRACTICE_API AMyPaperMonster : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPaperMonster();

	virtual void Tick(float DeltaTime) override;

	void InitTarget(AMyPaperCharacter * InTarget, bool bUseDistance, float InRadius);
    
    void SetMoveDirectionX(float DirX); // SpawnManager가 스폰 직후 방향을 고정 설정

    void StartWalkSound(USoundBase* WalkSound);

    UBoxComponent* GetHitBox() const { return HitBox; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void UpdateMovement(float DeltaTime);
    void SetState(EMonsterState NewState);
    void FaceToTarget();

    void EnableDetection();

	// When Monster and Player overlap, call this function
    UFUNCTION()
    void OnHitBoxOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UPROPERTY(VisibleAnywhere, Category = "Monster|Visual")
    UPaperFlipbookComponent* Flipbook;

    UPROPERTY(EditAnywhere, Category = "Monster|Anim")
    UPaperFlipbook* FB_Idle;

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

    UPROPERTY()
    AMyPaperCharacter * Target = nullptr;

    EMonsterState State = EMonsterState::Idle;

    // 시야 감지용 LineTrace 채널(필요 시 변경)
    ECollisionChannel LOSChannel = ECC_Visibility;

    // +1 = 오른쪽으로 직진, -1 = 왼쪽으로 직진
    float MoveDirX = 1.f;
private:
    UPROPERTY(VisibleAnywhere, Category = "Sound")
    UAudioComponent* WalkAudioComp;

    UPROPERTY(EditAnywhere, Category = "Sound")
    USoundBase* WalkSound;

    UPROPERTY(EditAnywhere, Category = "Sound")
    USoundBase* ShoutSound;

    UPROPERTY()
    bool bHasKilledPlayer = false;

};
