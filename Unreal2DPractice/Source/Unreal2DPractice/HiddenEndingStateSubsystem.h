#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HiddenEndingStateSubsystem.generated.h"

UCLASS()
class UNREAL2DPRACTICE_API UHiddenEndingStateSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    void BeginHiddenEndingReturnFlow(FName FinalTargetLevelName);
    void NotifySubLevelChanged(FName PreviousLevel, FName NextLevel);
    bool ConsumeImmediateReturn();
    bool ConsumeFinalReturnForLevel(FName LevelName);

private:
    UPROPERTY()
    bool bPendingImmediateReturn = false;

    UPROPERTY()
    bool bPendingFinalReturn = false;

    UPROPERTY()
    FName PendingFinalReturnLevelName = NAME_None;
};
