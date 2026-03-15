#include "EndingChaseManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "AudioManager.h"
#include "MyGameInstance.h"
#include "GameBGMData.h"

AEndingChaseManager::AEndingChaseManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEndingChaseManager::BeginPlay()
{
    Super::BeginPlay();

    FWorldDelegates::LevelAddedToWorld.AddUObject(
        this,
        &AEndingChaseManager::OnLevelLoaded);
}

void AEndingChaseManager::OnLevelLoaded(ULevel* InLevel, UWorld* InWorld)
{
    if (!Route.IsValidIndex(CurrentStageIndex)) return;
    if (!MonsterClass) return;

    FString LoadedLevelName = InLevel->GetOuter()->GetName();
    const FChaseStage& Stage = Route[CurrentStageIndex];

    if (!LoadedLevelName.Contains(Stage.LevelName.ToString()))
        return;

    GetWorld()->GetTimerManager().SetTimer(
        FirstSpawnTimerHandle,
        this,
        &AEndingChaseManager::StartChase,
        FirstSpawnDelay,
        false);
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
    ActiveMonster->SetManager(this);
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

    if (Route.IsValidIndex(CurrentStageIndex))
    {
        SpawnCurrentStage();
    }
}