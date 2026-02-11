#include "EndingFlowManager.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"

AEndingFlowManager::AEndingFlowManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEndingFlowManager::BeginPlay()
{
    Super::BeginPlay();
}

void AEndingFlowManager::RequestEnding(EEndingType Type, const FVector& TeleportLocation)
{
    PendingTeleportLocation = TeleportLocation;

    PendingLevelName =
        (Type == EEndingType::Hidden)
        ? TEXT("HiddenEnding")
        : TEXT("NormalEnding");

    UnloadCurrentSubLevel();
    LoadEndingLevel();
}

void AEndingFlowManager::UnloadCurrentSubLevel()
{
    if (CurrentSubLevelName.IsNone())
        return;

    UGameplayStatics::UnloadStreamLevel(
        this,
        CurrentSubLevelName,
        FLatentActionInfo(),
        false
    );

    CurrentSubLevelName = NAME_None;
}

void AEndingFlowManager::LoadEndingLevel()
{
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = FName("OnEndingLevelLoaded");
    LatentInfo.UUID = 1;
    LatentInfo.Linkage = 0;

    UGameplayStatics::LoadStreamLevel(
        this,
        PendingLevelName,
        true,
        false,
        LatentInfo
    );
}

void AEndingFlowManager::OnEndingLevelLoaded()
{
    CurrentSubLevelName = PendingLevelName;
    MovePlayer();
}

void AEndingFlowManager::MovePlayer()
{
    AMyPaperCharacter* Player =
        Cast<AMyPaperCharacter>(
            UGameplayStatics::GetPlayerCharacter(this, 0)
        );

    if (!Player)
        return;

    Player->SetActorLocation(
        PendingTeleportLocation,
        false,
        nullptr,
        ETeleportType::TeleportPhysics
    );
}
