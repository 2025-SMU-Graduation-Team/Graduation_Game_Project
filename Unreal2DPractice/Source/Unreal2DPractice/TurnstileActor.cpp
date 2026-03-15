// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnstileActor.h"
#include "MyPaperCharacter.h"
#include "AudioManager.h"
#include "GameSFXData.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"
#include "TimerManager.h"

// Sets default values
ATurnstileActor::ATurnstileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATurnstileActor::Interact()
{
    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

    if (!Player) return;

    if (Player->EquippedItem == EItemType::Card)  
    {
        AAudioManager* AudioManager =
            Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));
        UMyGameInstance* GI = GetWorld()->GetGameInstance<UMyGameInstance>();

        if (AudioManager && GI && GI->SFXData && GI->SFXData->TurnstileBeep)
        {
            AudioManager->PlaySFX2D(GI->SFXData->TurnstileBeep);
        }

        GetWorldTimerManager().ClearTimer(OpenSoundTimerHandle);
        GetWorldTimerManager().SetTimer(
            OpenSoundTimerHandle,
            this,
            &ATurnstileActor::PlayOpenSoundAndUnlock,
            OpenSoundDelay,
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Need a card"));
    }
}

// Called when the game starts or when spawned
void ATurnstileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATurnstileActor::PlayOpenSoundAndUnlock()
{
    AAudioManager* AudioManager =
        Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));
    UMyGameInstance* GI = GetWorld()->GetGameInstance<UMyGameInstance>();

    if (AudioManager && GI && GI->SFXData && GI->SFXData->TurnstileOpen)
    {
        AudioManager->PlaySFX(GI->SFXData->TurnstileOpen, GetActorLocation());
    }

    SetActorEnableCollision(false);
    UE_LOG(LogTemp, Warning, TEXT("Gate opened"));
}



