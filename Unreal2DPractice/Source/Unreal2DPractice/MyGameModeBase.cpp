// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyPaperCharacter.h"
#include "AudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "SubLevelTaskManager.h"
#include "Camera/PlayerCameraManager.h"
#include "Misc/PackageName.h"

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = nullptr;
    AudioManagerClass = AAudioManager::StaticClass();
}

void AMyGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    EnsureAudioManager();

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

    if (PC && PC->PlayerCameraManager)
    {
        PC->PlayerCameraManager->StopCameraFade();
        PC->PlayerCameraManager->StartCameraFade(
            1.f, 0.f, 0.f, FLinearColor::Black, false, false
        );
    }
}

void AMyGameModeBase::EnsureAudioManager()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    if (UGameplayStatics::GetActorOfClass(World, AAudioManager::StaticClass()))
    {
        return;
    }

    UClass* SpawnClass = AudioManagerClass.Get();
    if (!SpawnClass)
    {
        SpawnClass = AAudioManager::StaticClass();
    }

    if (!SpawnClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MyGameModeBase] AudioManagerClass is null."));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AAudioManager* SpawnedAudioManager = World->SpawnActor<AAudioManager>(
        SpawnClass,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        SpawnParams);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("[MyGameModeBase] AudioManager ensured. Existing=%s Spawned=%s"),
        TEXT("false"),
        *GetNameSafe(SpawnedAudioManager));
}

void AMyGameModeBase::OnLevelLoaded(ULevel* LoadedLevel, UWorld* World)
{
    if (!LoadedLevel || !World)
    {
        return;
    }

    const FString LevelName = LoadedLevel->GetOutermost()->GetName();
    const FString ShortLevelName = FPackageName::GetShortName(LevelName);
    UE_LOG(LogTemp, Warning, TEXT("Level Added: %s"), *LevelName);

    if (ShortLevelName == TEXT("Subway"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Subway Loaded Detected!"));
        if (USubLevelTaskManager* TaskManager = GetGameInstance()->GetSubsystem<USubLevelTaskManager>())
        {
            TaskManager->OnSubLevelEntered();
        }
    }
}
