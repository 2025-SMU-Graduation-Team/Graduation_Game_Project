#include "HiddenEndingStateSubsystem.h"

void UHiddenEndingStateSubsystem::BeginHiddenEndingReturnFlow(FName FinalTargetLevelName)
{
    bPendingImmediateReturn = false;
    bPendingFinalReturn = !FinalTargetLevelName.IsNone();
    PendingFinalReturnLevelName = FinalTargetLevelName;
}

void UHiddenEndingStateSubsystem::NotifySubLevelChanged(FName PreviousLevel, FName NextLevel)
{
    if (PreviousLevel == TEXT("HiddenEnding") && NextLevel != TEXT("HiddenEnding"))
    {
        bPendingImmediateReturn = true;
    }
}

bool UHiddenEndingStateSubsystem::ConsumeImmediateReturn()
{
    if (!bPendingImmediateReturn)
    {
        return false;
    }

    bPendingImmediateReturn = false;
    return true;
}

bool UHiddenEndingStateSubsystem::ConsumeFinalReturnForLevel(FName LevelName)
{
    if (!bPendingFinalReturn || PendingFinalReturnLevelName != LevelName)
    {
        return false;
    }

    bPendingFinalReturn = false;
    PendingFinalReturnLevelName = NAME_None;
    return true;
}
