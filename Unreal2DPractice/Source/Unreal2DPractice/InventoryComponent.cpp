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

bool UInventoryComponent::IsInventoryFull() const
{
	return Items.Num() >= MaxInventorySize;
}

void UInventoryComponent::RequestPickup(AItemActor* Item, bool bIsWallet)
{
	if (!InventoryWidget && !Item) return;

	if (IsInventoryFull())
	{
		AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(GetOwner());
		Player->bEnableMovement = true;

		InventoryWidget->ShowInventoryFullPopup();
		return;
	}

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
	AddItem(PendingItem);

	/*if (PendingItem->ItemType == EItemType::Wallet)
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
	}*/

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

void UInventoryComponent::SelectSlot(int32 Index)
{
	if (!Items.IsValidIndex(Index))
	{
		return;
	}

	if (SelectedInvenIndex == Index)
	{
		SelectedInvenIndex = INDEX_NONE;
		InventoryWidget->HideItemInfoPopup();
		return;
	}

	SelectedInvenIndex = Index;
	InventoryWidget->ShowItemInfoPopup(Items[Index].ItemName);
}

void UInventoryComponent::UseSelectedItem()
{
	if (!Items.IsValidIndex(SelectedInvenIndex)) return;

	FInventoryItem& SelectedItem = Items[SelectedInvenIndex];

	if (!SelectedItem.bIsWallet)
	{
		UE_LOG(LogTemp, Log, TEXT("Item is not a wallet."));
		return;
	}

	SelectedItem.bIsWallet = false;
	SelectedItem.ItemName = FText::FromString(TEXT("Card"));
	SelectedItem.Icon = CardIcon;

	UpdateInventoryUI();

	if (InventoryWidget)
	{
		InventoryWidget->ShowItemInfoPopup(SelectedItem.ItemName);
	}
}

void UInventoryComponent::AddItem(AItemActor* ItemActor)
{
	if (!ItemActor) return;

	FInventoryItem NewItem;
	NewItem.Icon = ItemActor->ItemIcon;
	NewItem.ItemName = FText::FromName(ItemActor->ItemName);
	NewItem.bIsWallet = (ItemActor->ItemType == EItemType::Wallet);

	Items.Add(NewItem);
	UpdateInventoryUI();
}

void UInventoryComponent::UpdateInventoryUI()
{
	if (InventoryWidget)
	{
		InventoryWidget->UpdateInventory(Items);
	}
}
