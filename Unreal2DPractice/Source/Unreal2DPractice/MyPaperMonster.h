// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPaperMonster.generated.h"


class UPaperFlipbookComponent;
class UPaperFlipbook;
class AMyPaperCharacter; //class APlayer2DCharacter;

UENUM(BlueprintType)
enum class EMonsterState : uint8 { Idle, Walk, Attack };


UCLASS()
class UNREAL2DPRACTICE_API AMyPaperMonster : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPaperMonster();

	virtual void Tick(float DeltaTime) override;

	// 스포너가 타겟/옵션을 넣어줌
	void InitTarget(AMyPaperCharacter * InTarget, bool bUseDistance, float InRadius);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void UpdateMovement(float DeltaTime);
    void UpdateDetection();

    void SetState(EMonsterState NewState);
    void FaceToTarget();

    // 몬스터 스프라이트 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Monster|Visual")
    UPaperFlipbookComponent* Flipbook;

    UPROPERTY(EditAnywhere, Category = "Monster|Anim")
    UPaperFlipbook* FB_Idle;
    UPROPERTY(EditAnywhere, Category = "Monster|Anim")
    UPaperFlipbook* FB_Walk;
    UPROPERTY(EditAnywhere, Category = "Monster|Anim")
    UPaperFlipbook* FB_Attack;

    UPROPERTY(EditAnywhere, Category = "AI")
    float MoveSpeed = 300.f;

    UPROPERTY(EditAnywhere, Category = "Detect")
    bool bDetectByDistance = true;

    UPROPERTY(EditAnywhere, Category = "Detect", meta = (EditCondition = "bDetectByDistance"))
    float DetectRadius = 800.f;

    UPROPERTY()
    AMyPaperCharacter * Target = nullptr;

    EMonsterState State = EMonsterState::Idle;

    // 시야 감지용 LineTrace 채널(필요 시 변경)
    ECollisionChannel LOSChannel = ECC_Visibility;

//public:	
// 
	//Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
