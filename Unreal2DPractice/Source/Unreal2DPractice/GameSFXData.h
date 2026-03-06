#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"
#include "GameSFXData.generated.h"

UCLASS(BlueprintType)
class UNREAL2DPRACTICE_API UGameSFXData : public UDataAsset
{
    GENERATED_BODY()

public:

    /* UI */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* BtnClick;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* BtnClickLow;

    /* Minigame */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* BtnWrong;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* BeepHigh;

    /* Monster */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* MonsterWalk;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* MonsterAttack;

    /* Subway */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* SubwayBrake;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* SubwayBell;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* DoorOpen;

    /* Player */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerWalk;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerStairWalk;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerPickup;
};