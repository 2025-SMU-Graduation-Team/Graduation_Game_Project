#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControlRoomVolume.generated.h"

class UBoxComponent;
class AMonsterSpawnManager;

UCLASS()
class UNREAL2DPRACTICE_API AControlRoomVolume : public AActor
{
	GENERATED_BODY()
	
public:
	AControlRoomVolume();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Box;

	// 레벨에 있는 MonsterSpawnManager를 자동으로 찾아 캐싱
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

public:	
	void SetControlRoomFlag(bool bInControlRoom);

};
