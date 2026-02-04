#include "EndingFlowManager.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"

void AEndingFlowManager::RequestEnding(EEndingType Type)
{
    PendingEnding = Type;

    if (Type == EEndingType::Hidden)
    {
        PendingLevelName = TEXT("HiddenEnding");
        PendingTeleportLocation = FVector(6230, -14280, -216);
    }
    else
    {
        PendingLevelName = TEXT("NormalEnding");
        PendingTeleportLocation = FVector(6230, -14280, -216);
    }

    FLatentActionInfo Info;
    Info.CallbackTarget = this;
    Info.ExecutionFunction = "OnEndingLevelLoaded";
    Info.Linkage = 0;
    Info.UUID = __LINE__;

    UGameplayStatics::LoadStreamLevel(
        this,
        PendingLevelName,
        true,
        false,
        Info
    );
}

void AEndingFlowManager::OnEndingLevelLoaded()
{
    UE_LOG(LogTemp, Log, TEXT("OnEndingLevelLoaded"));
    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

    if (!Player) return;

    Player->SetActorLocation(PendingTeleportLocation, false, nullptr, ETeleportType::TeleportPhysics);
}
