// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"
#include "Components/BoxComponent.h"
#include "MyPaperCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootScene);
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActor::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AInteractableActor::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void AInteractableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor))
	{
		CachedPlayer = Player;
		Player->CurrentInteractable = this;
		PC = Cast<APlayerController>(Player->GetController());
	}
}

void AInteractableActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor))
	{
		if (Player->CurrentInteractable == this)
		{
			Player->CurrentInteractable = nullptr;
		}
		CachedPlayer = nullptr;
		PC = nullptr;
	}
}

void AInteractableActor::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Base Interact called (no implementation)."));
}

