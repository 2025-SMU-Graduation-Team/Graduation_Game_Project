// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelChangeActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "MyPaperCharacter.h"
#include "SubLevelTaskManager.h"
#include "LevelTransitionManager.h"

ALevelChangeActor::ALevelChangeActor()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelChangeActor::OnOverlapBegin);
}

void ALevelChangeActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
    if (!Player)
        return;

    CachedPlayer = Player;

    CachedPlayer->SetActorLocation(TargetTeleportLocation);

    ALevelTransitionManager* Manager = ALevelTransitionManager::Get(GetWorld());
    if (Manager)
    {
        Manager->ChangeSubLevel(NextLevelName);
    }
}
