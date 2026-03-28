#include "EndingTriggerBox.h"

#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"

AEndingTriggerBox::AEndingTriggerBox()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AEndingTriggerBox::OnTriggerBeginOverlap);
}

void AEndingTriggerBox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void AEndingTriggerBox::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (bHasTriggered && bTriggerOnce)
	{
		return;
	}

	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	if (!PC)
	{
		return;
	}

	bHasTriggered = true;

	if (bTriggerOnce && TriggerBox)
	{
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (!ActiveWidget && EndingWidgetClass)
	{
		ActiveWidget = CreateWidget<UUserWidget>(PC, EndingWidgetClass);
		if (ActiveWidget)
		{
			ActiveWidget->AddToViewport(WidgetZOrder);
		}
	}

	if (ActiveWidget)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(ActiveWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
	}

	PC->bShowMouseCursor = bShowMouseCursorOnTrigger;

	if (bPauseGameOnTrigger)
	{
		UGameplayStatics::SetGamePaused(this, true);
	}
}
