#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioManager.generated.h"

class UAudioComponent;
class UGameSFXData;
class UGameBGMData;

UCLASS()
class UNREAL2DPRACTICE_API AAudioManager : public AActor
{
	GENERATED_BODY()

public:

	AAudioManager();

	UFUNCTION(BlueprintCallable)
	void PlaySFX(USoundBase* Sound, FVector Location);

	UFUNCTION(BlueprintCallable)
	void PlaySFX2D(USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	void PlayBGM(USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	void StopBGM();

protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	UAudioComponent* BGMComponent;

	UGameSFXData* SFXData;
	UGameBGMData* BGMData;
};