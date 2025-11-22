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

    UWorld* World = GetWorld();

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
    if (LoadedLevel->GetOuter()->GetName() == "Subway")
    {
        USubLevelTaskManager::Get(World)->OnSubLevelEntered();
    }
}