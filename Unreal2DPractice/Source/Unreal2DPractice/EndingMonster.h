// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPaperMonster.h"
#include "EndingMonster.generated.h"

class AEndingChaseManager;

UENUM()
enum class EObstacleState : uint8
{
	None,
	WaitingBeforeBreak,
	Breaking,
	WaitingAfterBreak
};

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

	void StopMonster();

	float MoveDirection = 1.f;

private:
	FVector EndLocation;
	FVector TurnLocation;

	UPROPERTY()
	AEndingChaseManager* Manager = nullptr;

	bool bEndTriggered = false;
	bool bTurnedAtPoint = false;

	float FastSpeed = 600.f;

	void CheckTurnPoint();
	void CheckEndPoint();

	void HandleObstacle(float DeltaTime);
	bool IsFrontBlocked(FHitResult& OutHit) const;

	EObstacleState ObstacleState = EObstacleState::None;
	float ObstacleTimer = 0.f;

	UPROPERTY(EditAnywhere, Category = "Obstacle")
	float WaitBeforeBreak = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Obstacle")
	float BreakDuration = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Obstacle")
	float WaitAfterBreak = 0.6f;

	UPROPERTY()
	AActor* CurrentObstacle = nullptr;
};
