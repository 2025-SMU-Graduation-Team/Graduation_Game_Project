#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"
#include "GameSoundData.generated.h"

UCLASS(BlueprintType)
class UNREAL2DPRACTICE_API UGameSoundData : public UDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    USoundBase* UI_Click;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
    USoundBase* Monster_Walk;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
    USoundBase* Monster_Attack;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
    USoundBase* BGM_Main;
};