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
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* BtnClickHigh;

    /* Minigame */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Mini_BtnWrong;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Mini_BeepSoundHigh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Mini_SubwaySelect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Mini_PanelGageup;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Mini_LineCutting;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Mini_BtnClick_simple;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Mini_BtnClick_wrong_long;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* Mini_BtnClick_wrong_short;

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* TurnstileBeep;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* TurnstileOpen;

    /* Player */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerWalk;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerStairWalk;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerJumpLanding;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerHide;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerPickup;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerPickupGain;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerChangeTool;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerDeathScream;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* PlayerDeathNPC;
};
