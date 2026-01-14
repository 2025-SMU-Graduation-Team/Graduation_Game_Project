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

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* TriggerBox;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* LeftSpawnPoint;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* RightSpawnPoint;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    TSubclassOf<AMyPaperMonster> MonsterClass;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    float SpawnDelay = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Spawner|Sound")
    USoundBase* MonsterWalkSound;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    bool bOneShot = true;

    bool bHasSpawned = false;
    bool bSpawnFromLeft = true;

    UPROPERTY()
    AMyPaperCharacter* CachedPlayer = nullptr;

    FTimerHandle TimerHandle_Spawn;

    UFUNCTION()
    void OnTriggerBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    void SpawnMonster();
};