// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelChangeActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "MyPaperCharacter.h"
#include "SubLevelTaskManager.h"

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
	if (AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor))
	{
		MoveToLevel(NextLevelName);
	}
}

void ALevelChangeActor::MoveToLevel(FName LevelName)
{
	if (LevelName == FName("SongChaegang"))
	{
		UGameplayStatics::OpenLevel(this, LevelName);
		return;
	}

	UGameplayStatics::LoadStreamLevel(this, LevelName, true, false, FLatentActionInfo());
}
