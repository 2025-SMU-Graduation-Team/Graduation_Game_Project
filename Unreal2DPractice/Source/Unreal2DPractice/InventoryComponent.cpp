#include "InventoryComponent.h"
#include "ItemInventoryWidget.h"
#include "ItemActor.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"


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

void UInventoryComponent::RequestPickup(AItemActor* Item)
{
	if (!Item) return;

	PendingItem = Item;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	if (InventoryWidget)
	{
		InventoryWidget->ShowConfirmPopup(Item->ItemIcon);
	}
}

void UInventoryComponent::ConfirmPickupYes()
{
	if (!PendingItem) return;

	AddItem(PendingItem->ItemIcon);
	PendingItem->Destroy();
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
