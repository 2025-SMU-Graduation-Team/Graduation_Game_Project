// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyPaperCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "SubLevelTaskManager.h"

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = nullptr;
}

void AMyGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    FWorldDelegates::LevelAddedToWorld.AddUObject(
        this, &AMyGameModeBase::OnLevelLoaded);


    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    AMyPaperCharacter* ExistingPlayer =
        Cast<AMyPaperCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPaperCharacter::StaticClass()));

    if (PC && ExistingPlayer)
    {
        PC->Possess(ExistingPlayer);
        UE_LOG(LogTemp, Log, TEXT("Possessed existing PaperCharacter: %s"), *ExistingPlayer->GetName());
    }
}

void AMyGameModeBase::OnLevelLoaded(ULevel* LoadedLevel, UWorld* World)
{
    if (LoadedLevel && LoadedLevel->GetOuter() && LoadedLevel->GetOuter()->GetName() == "Subway")
    {

        if (!LoadedLevel || !World) return;

        FString LevelName = LoadedLevel->GetOutermost()->GetName();
        UE_LOG(LogTemp, Warning, TEXT("Level Added: %s"), *LevelName);

        if (LevelName.Contains(TEXT("Subway")))
        {
            UE_LOG(LogTemp, Warning, TEXT("Subway Loaded Detected!"));
            USubLevelTaskManager::Get(World)->OnSubLevelEntered();
        }
    }
}