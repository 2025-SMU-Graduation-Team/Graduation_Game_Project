// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPaperMonster.h"
#include "EndingMonster.generated.h"

class AEndingChaseManager;

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
	FVector EndLocation;
	FVector TurnLocation;

	UPROPERTY()
	AEndingChaseManager* Manager = nullptr;

	bool bEndTriggered = false;

	float FastSpeed = 600.f;

	void CheckTurnPoint();
	void CheckEndPoint();
};
