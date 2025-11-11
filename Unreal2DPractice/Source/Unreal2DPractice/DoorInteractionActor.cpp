// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionActor.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/LevelStreamingDynamic.h"
#include "MyPaperCharacter.h"

// Sets default values
ADoorInteractionActor::ADoorInteractionActor()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADoorInteractionActor::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADoorInteractionActor::OnOverlapEnd);

	bPlayerInRange = false;
}

// Called when the game starts or when spawned
void ADoorInteractionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoorInteractionActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass())) {
		bPlayerInRange = true;
		CachedPlayer = Cast<AMyPaperCharacter>(OtherActor);

		if (CachedPlayer)
		{
			AMyPaperCharacter* MyPlayer = Cast<AMyPaperCharacter>(CachedPlayer);
			if (MyPlayer)
			{
				UE_LOG(LogTemp, Log, TEXT("MyPlayer is cached"));
				MyPlayer->CurrentDoor = this;
			}
		}
	}
}

void ADoorInteractionActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass())) {
		bPlayerInRange = false;

		if (CachedPlayer)
		{
			AMyPaperCharacter* MyPlayer = Cast<AMyPaperCharacter>(CachedPlayer);
			if (MyPlayer && MyPlayer->CurrentDoor == this)
			{
				MyPlayer->CurrentDoor = nullptr;
			}

			CachedPlayer = nullptr;
		}
	}
}

void ADoorInteractionActor::Interact()
{
	if (!bPlayerInRange) return;

	UE_LOG(LogTemp, Log, TEXT("PlayerInRange: %s"), bPlayerInRange ? TEXT("True") : TEXT("False"));
	UGameplayStatics::LoadStreamLevel(this, TargetLevelName, true, true, FLatentActionInfo());

	if (CachedPlayer)
	{
		CachedPlayer->SetActorLocation(TargetTeleportLocation);
	}
}

// Called every frame
void ADoorInteractionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

