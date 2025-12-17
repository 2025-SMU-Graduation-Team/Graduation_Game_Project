// MyMonsterSpawner.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyMonsterSpawner.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;
class AMyPaperMonster;
class AMyPaperCharacter;

UCLASS()
class UNREAL2DPRACTICE_API AMyMonsterSpawner : public AActor
{
    GENERATED_BODY()

public:
    AMyMonsterSpawner();

protected:
    virtual void BeginPlay() override;

    // 컴포넌트들
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* TriggerBox;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* LeftSpawnPoint;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* RightSpawnPoint;

    // 스폰 설정
    UPROPERTY(EditAnywhere, Category = "Spawner")
    TSubclassOf<AMyPaperMonster> MonsterClass;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    float SpawnDelay = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Spawner|Sound")
    USoundBase* Sound_Left;

    UPROPERTY(EditAnywhere, Category = "Spawner|Sound")
    USoundBase* Sound_Right;

    // 한 번만 발동할지 여부
    UPROPERTY(EditAnywhere, Category = "Spawner")
    bool bOneShot = true;

    bool bHasSpawned = false;      // 이미 발동했는지
    bool bSpawnFromLeft = true;    // 이번에 왼쪽에서 나올지

    UPROPERTY()
    AMyPaperCharacter* CachedPlayer = nullptr;

    FTimerHandle TimerHandle_Spawn;

    // 트리거 진입 이벤트
    UFUNCTION()
    void OnTriggerBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    // 딜레이 후 실제 몬스터 스폰
    void SpawnMonster();
};