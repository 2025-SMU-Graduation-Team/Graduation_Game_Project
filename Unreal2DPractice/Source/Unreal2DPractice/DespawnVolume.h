#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DespawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class UNREAL2DPRACTICE_API ADespawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ADespawnVolume();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Box;

	UFUNCTION()
	void OnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
