// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPaperMonster.h"
#include "EndingMonster.generated.h"

class AEndingChaseManager;
class AMyPaperCharacter;

UCLASS()
class UNREAL2DPRACTICE_API AEndingMonster : public AMyPaperMonster
{
	GENERATED_BODY()
	
public:
	AEndingMonster();

	void SetMoveDirection(float Dir);

	void SetEndLocation(const FVector& InLocation);
	void SetTurnLocation(const FVector& InLocation);
	void SetManager(class AEndingChaseManager* InManager);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnHitBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	float MoveDirection = 1.f;

private:
	void CheckTurnPoint();
	void CheckEndPoint();
	bool HandleObstacle(float DeltaTime);
	bool TryKillPlayerAhead();
	bool TryKillPlayer(AMyPaperCharacter* Player);
	bool IsFrontBlocked(FHitResult& OutHit) const;
	bool IsBreakableHit(const FHitResult& Hit) const;

	FVector EndLocation;
	FVector TurnLocation;

	UPROPERTY()
	AEndingChaseManager* Manager = nullptr;

	UPROPERTY()
	AActor* CurrentObstacle = nullptr;

	bool bEndTriggered = false;
	bool bTurnedAtPoint = false;
	bool bBreakingObstacle = false;

	float FastSpeed = 600.f;
	float ObstacleTimer = 0.f;

	UPROPERTY(EditAnywhere, Category = "Obstacle")
	float WaitBeforeBreak = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Obstacle")
	float BreakDuration = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Player")
	float PlayerDetectDistance = 200.f;
};
