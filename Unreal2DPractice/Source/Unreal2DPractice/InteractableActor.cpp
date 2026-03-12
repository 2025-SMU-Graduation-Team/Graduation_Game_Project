// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"
#include "Components/BoxComponent.h"
#include "MyPaperCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "MyGameInstance.h"
#include "InteractWidget.h"

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

	PrimaryActorTick.bCanEverTick = true;
}

void AInteractableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!TriggerBox || !TriggerBox->IsCollisionEnabled())
		return;

	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player || ActiveWidget) return;

	CachedPlayer = Player;
	Player->CurrentInteractable = this;

	PC = Cast<APlayerController>(Player->GetController());
	if (!PC) return;

	UMyGameInstance* GI = GetWorld()->GetGameInstance<UMyGameInstance>();
	if (!GI || !GI->DefaultInteractWidget) return;

	ActiveWidget = CreateWidget<UUserWidget>(PC, GI->DefaultInteractWidget);
	if (!ActiveWidget) return;

	ActiveWidget->AddToViewport();
	UpdateWidgetPosition();

	if (UInteractWidget* InteractWidget = Cast<UInteractWidget>(ActiveWidget))
	{
		InteractWidget->SetInteractInfo(GetDisplayInteractKey(), InteractText);
	}
}

void AInteractableActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player) return;

	if (Player->CurrentInteractable == this)
	{
		Player->CurrentInteractable = nullptr;
	}

	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}

	CachedPlayer = nullptr;
	PC = nullptr;
}

void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ActiveWidget)
	{
		UpdateWidgetPosition();
	}
}

void AInteractableActor::UpdateWidgetPosition()
{
	if (!CachedPlayer || !PC || !ActiveWidget) return;

	FVector WorldPos = CachedPlayer->GetActorLocation();
	WorldPos += FVector(-100.f, 0.f, 70.f);

	FVector2D ScreenPos;
	PC->ProjectWorldLocationToScreen(WorldPos, ScreenPos);

	ActiveWidget->SetPositionInViewport(ScreenPos, true);
}

void AInteractableActor::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Base Interact called (no implementation)."));
}

void AInteractableActor::InteractFromEnterKey()
{
	Interact();
}

bool AInteractableActor::CanInteractFromEnterKey() const
{
	return bAllowEnterKeyInteraction;
}

FText AInteractableActor::GetDisplayInteractKey() const
{
	if (bAllowEnterKeyInteraction && !AlternateInteractKey.IsEmpty())
	{
		return AlternateInteractKey;
	}

	return InteractKey;
}
