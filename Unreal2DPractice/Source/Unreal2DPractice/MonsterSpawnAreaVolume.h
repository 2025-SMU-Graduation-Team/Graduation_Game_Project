#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawnAreaVolume.generated.h"

class UBoxComponent;
class AMonsterSpawnManager;

UCLASS()
class UNREAL2DPRACTICE_API AMonsterSpawnAreaVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AMonsterSpawnAreaVolume();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Box;

	UPROPERTY()
	AMonsterSpawnManager* SpawnManager = nullptr;

	UFUNCTION()
	void OnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnBoxEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};
