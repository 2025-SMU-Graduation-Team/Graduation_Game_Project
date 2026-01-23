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

    UPROPERTY()
    AMyPaperCharacter* CachedPlayer = nullptr;

    UFUNCTION()
    void OnTriggerBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);
};