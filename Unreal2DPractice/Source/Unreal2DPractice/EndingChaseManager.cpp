#include "EndingChaseManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "AudioManager.h"
#include "MyGameInstance.h"
#include "GameBGMData.h"
#include "Engine/Level.h"
#include "VerticalTraversalActor.h"

AEndingChaseManager::AEndingChaseManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEndingChaseManager::BeginPlay()
{
    Super::BeginPlay();

    if (!ManagedTraversalActor)
    {
        ManagedTraversalActor = Cast<AVerticalTraversalActor>(
            UGameplayStatics::GetActorOfClass(this, AVerticalTraversalActor::StaticClass())
        );
    }

    SetCurrentStageTraversalEnabled(false);

    FWorldDelegates::LevelAddedToWorld.AddUObject(
        this,
        &AEndingChaseManager::OnLevelLoaded);

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (ULevel* LoadedLevel : World->GetLevels())
    {
        if (!LoadedLevel)
        {
            continue;
        }

        const UPackage* LevelPackage = LoadedLevel->GetPackage();
        if (!LevelPackage)
        {
            continue;
        }

        if (TryStartChaseForLevelName(LevelPackage->GetName()))
        {
            break;
        }
    }
}

void AEndingChaseManager::OnLevelLoaded(ULevel* InLevel, UWorld* InWorld)
{
    if (!InLevel || InWorld != GetWorld())
    {
        return;
    }

    const UPackage* LevelPackage = InLevel->GetPackage();
    if (!LevelPackage)
    {
        return;
    }

    TryStartChaseForLevelName(LevelPackage->GetName());
}

bool AEndingChaseManager::TryStartChaseForLevelName(const FString& LoadedLevelName)
{
    if (bHasStartedCurrentStage)
    {
        return false;
    }

    if (!Route.IsValidIndex(CurrentStageIndex)) return false;
    if (!MonsterClass) return false;

    const FChaseStage& Stage = Route[CurrentStageIndex];

    if (!LoadedLevelName.Contains(Stage.LevelName.ToString()))
        return false;

    bHasStartedCurrentStage = true;
    SetCurrentStageTraversalEnabled(false);

    GetWorld()->GetTimerManager().SetTimer(
        FirstSpawnTimerHandle,
        this,
        &AEndingChaseManager::StartChase,
        FirstSpawnDelay,
        false);

    return true;
}

void AEndingChaseManager::StartChase()
{
    AAudioManager* AudioManager =
        Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));

    UMyGameInstance* GI =
        Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

    if (AudioManager && GI && GI->BGMData && GI->BGMData->Chasing)
    {
        AudioManager->StopBGM();
        AudioManager->PlayBGM(GI->BGMData->Chasing);
    }

    SpawnCurrentStage();
}

void AEndingChaseManager::SpawnCurrentStage()
{
    if (!Route.IsValidIndex(CurrentStageIndex)) return;
    if (!MonsterClass) return;

    const FChaseStage& Stage = Route[CurrentStageIndex];

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ActiveMonster = GetWorld()->SpawnActor<AEndingMonster>(
        MonsterClass,
        Stage.SpawnLocation,
        FRotator::ZeroRotator,
        Params);

    if (!ActiveMonster) return;

    ActiveMonster->SetMoveDirection(Stage.MoveDirection);
    ActiveMonster->SetEndLocation(Stage.EndLocation);
    ActiveMonster->SetTurnLocation(Stage.TurnLocation);
    ActiveMonster->SetCanKillHiddenPlayer(CurrentStageIndex > 0);
    ActiveMonster->SetManager(this);
    SetCurrentStageTraversalEnabled(true);
}

void AEndingChaseManager::NotifyReachedEnd(AEndingMonster* Monster)
{
    if (Monster != ActiveMonster) return;

    if (ActiveMonster)
    {
        ActiveMonster->Destroy();
        ActiveMonster = nullptr;
    }

    CurrentStageIndex++;
    bHasStartedCurrentStage = false;

    if (Route.IsValidIndex(CurrentStageIndex))
    {
        UWorld* World = GetWorld();
        if (!World)
        {
            return;
        }

        for (ULevel* LoadedLevel : World->GetLevels())
        {
            if (!LoadedLevel)
            {
                continue;
            }

            const UPackage* LevelPackage = LoadedLevel->GetPackage();
            if (!LevelPackage)
            {
                continue;
            }

            if (TryStartChaseForLevelName(LevelPackage->GetName()))
            {
                break;
            }
        }
    }
}

void AEndingChaseManager::SetCurrentStageTraversalEnabled(bool bEnabled)
{
    if (!ManagedTraversalActor)
    {
        return;
    }

    ManagedTraversalActor->SetTraversalEnabled(bEnabled);
}
