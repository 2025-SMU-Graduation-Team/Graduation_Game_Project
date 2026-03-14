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

	void PlaySFX(USoundBase* Sound, FVector Location);

	void PlaySFX2D(USoundBase* Sound);

	void PlayBGM(USoundBase* Sound);

	void StopBGM();

protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	UAudioComponent* BGMComponent;

	UGameSFXData* SFXData;
	UGameBGMData* BGMData;
};