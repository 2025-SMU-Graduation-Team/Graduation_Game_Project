#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"
#include "GameBGMData.generated.h"

UCLASS(BlueprintType)
class UNREAL2DPRACTICE_API UGameBGMData : public UDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* BasicBackground;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Chasing;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* GameClear;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* GameOver;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* GameOverViolin;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* GameOverVoice;
};