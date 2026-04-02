#include "EndingDirector.h"
#include "NormalEndingSequence.h"
#include "HiddenEndingSequence.h"

void AEndingDirector::StartNormalEnding(AMyPaperCharacter* Player, FVector TeleportLocation)
{
    if (!NormalClass || !Player)
        return;

    ANormalEndingSequence* Seq = GetWorld()->SpawnActor<ANormalEndingSequence>(NormalClass);

    if (Seq)
    {
        Seq->StartSequence(Player, TeleportLocation);
    }
}

void AEndingDirector::StartHiddenEnding(AMyPaperCharacter* Player, FVector TeleportLocation)
{
    if (!HiddenClass || !Player)
        return;
    UE_LOG(LogTemp, Log, TEXT("StartHiddenEnding"));

    AHiddenEndingSequence* Seq = GetWorld()->SpawnActor<AHiddenEndingSequence>(HiddenClass);

    if (Seq)
    {
        UE_LOG(LogTemp, Log, TEXT("Sequence exists"));

        Seq->StartSequence(Player, TeleportLocation);
    }
}
