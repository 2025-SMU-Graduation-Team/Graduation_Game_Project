#include "InventoryComponent.h"
#include "ItemInventoryWidget.h"
#include "ItemActor.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "MyPaperCharacter.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (InventoryWidgetClass)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			InventoryWidget = CreateWidget<UItemInventoryWidget>(PC, InventoryWidgetClass);
			if (InventoryWidget)
			{
				InventoryWidget->AddToViewport();
				InventoryWidget->HideConfirmPopup();
			}
		}
	}
}

void UInventoryComponent::RequestPickup(AItemActor* Item, bool bIsWallet)
{
	if (!InventoryWidget && !Item) return;

	PendingItem = Item;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	InventoryWidget->ShowConfirmPopup(Item->ItemIcon);
}

void UInventoryComponent::ConfirmPickupYes()
{
	if (!PendingItem) return;

	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(GetOwner());

	if (PendingItem->ItemType == EItemType::Wallet)
	{
		FVector SpawnLoc = PendingItem->GetActorLocation();
		GetWorld()->SpawnActor<AItemActor>(
			PendingItem->CardItemClass,
			SpawnLoc,
			PendingItem->GetActorRotation()
		);
	}
	else
	{
		AddItem(PendingItem->ItemIcon);
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	if (InventoryWidget)
	{
		InventoryWidget->HideConfirmPopup();
	}

	PendingItem->Destroy();
	PendingItem = nullptr;
	Player->bEnableMovement = true; 
}

void UInventoryComponent::ConfirmPickupNo()
{
	PendingItem = nullptr;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	if (InventoryWidget)
	{
		InventoryWidget->HideConfirmPopup();
	}

	if (AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(GetOwner()))
	{
		Player->bEnableMovement = true; 
	}
}

void UInventoryComponent::AddItem(UTexture2D* ItemIcon)
{
	if (!ItemIcon) return;

	Items.Add(ItemIcon);
	UpdateInventoryUI();
}

void UInventoryComponent::UpdateInventoryUI()
{
	if (InventoryWidget)
	{
		InventoryWidget->UpdateInventory(Items);
	}
}
