// MonsterSpawnManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerMessageWidget.h"
#include "MonsterSpawnManager.generated.h"

class AMyPaperMonster;
class AMyPaperCharacter;

UENUM(BlueprintType)
enum class ESpawnReason : uint8
{
	Random,
	Event
};

UCLASS()
class UNREAL2DPRACTICE_API AMonsterSpawnManager : public AActor
{
	GENERATED_BODY()

public:
	AMonsterSpawnManager();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AMyPaperMonster> MonsterClass;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (ClampMin = "0.1"))
	float MinSpawnInterval = 10.f;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (ClampMin = "0.1"))
	float MaxSpawnInterval = 20.f;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (ClampMin = "0.0"))
	float MinSpawnDistance = 700.f;

	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (ClampMin = "0.0"))
	float MaxSpawnDistance = 1400.f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnYOffset = -60.f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnZOffset = 60.f;

	// 기존 몬스터 InitTarget에 넘기는 감지 반경
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float DetectRadius = 800.f;

public:
	// --------------------
	// Control Room
	// --------------------
	// 관제실 내부 여부 (true면 스폰 금지)
	UFUNCTION(BlueprintCallable, Category = "Monster")
	void SetPlayerInControlRoom(bool bInControlRoom);

	// --------------------
	// Event Spawn
	// --------------------
	// 특정 아이템 상호작용 등 이벤트로 스폰 요청
	// 이미 몬스터가 있거나 관제실이면 무시
	UFUNCTION(BlueprintCallable)
	void RequestEventSpawn();

	// 관제실 문 닫힘 등으로 몬스터 즉시 제거하고 싶을 때
	UFUNCTION(BlueprintCallable, Category = "Monster")
	void ForceDespawnMonster();

	UFUNCTION(BlueprintCallable, Category = "Monster")
	void SetPlayerInSpawnArea(bool bInSpawnArea);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UPlayerMessageWidget> PlayerMessageWidgetClass;

private:
	FTimerHandle TimerHandle_Spawn;

	// 관제실 플래그
	bool bPlayerInControlRoom = false;

	// 현재 몬스터 추적 (없어도 되지만 있으면 편함)
	TWeakObjectPtr<AMyPaperMonster> CurrentMonster;

	void ScheduleNextSpawn();
	void TrySpawnRandom();
	void TrySpawn(ESpawnReason Reason);

	bool IsMonsterAlive();
	AMyPaperCharacter* GetPlayer() const;

	// 스폰 위치 계산 (좌/우 랜덤)
	FVector ComputeSpawnLocation(const AMyPaperCharacter* Player, bool bSpawnFromLeft, float SpawnDistance) const;

	bool bPlayerInSpawnArea = false;
};
