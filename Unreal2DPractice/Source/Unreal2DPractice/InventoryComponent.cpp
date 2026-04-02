#include "InventoryComponent.h"
#include "ItemInventoryWidget.h"
#include "ItemActor.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "MyPaperCharacter.h"
#include "AudioManager.h"
#include "MyGameInstance.h"
#include "GameSFXData.h"
#include "Engine/GameInstance.h"


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
				InventoryWidget->AddToViewport(100);
				InventoryWidget->HideConfirmPopup();
			}
		}
	}
}

bool UInventoryComponent::IsInventoryFull() const
{
	return Items.Num() >= MaxInventorySize;
}

void UInventoryComponent::RequestPickup(AItemActor* Item)
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

	if (PendingItem->ItemType == EItemType::Ghost)
	{
		InventoryWidget->HideConfirmPopup();
		PendingItem->Destroy();
		PendingItem = nullptr;

		Player->PlayDeath();
		return;
	}

	AddItem(PendingItem);

	AAudioManager* AudioManager =
		Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));

	UMyGameInstance* GI =
		Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	if (AudioManager && GI && GI->SFXData && GI->SFXData->PlayerPickup)
	{
		AudioManager->PlaySFX2D(GI->SFXData->PlayerPickup);
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	InventoryWidget->HideConfirmPopup();
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
	//InventoryWidget->ShowBorder(Index);

	if (!Items.IsValidIndex(Index))
	{
		return;
	}

	if (SelectedInvenIndex == Index)
	{
		SelectedInvenIndex = INDEX_NONE;
		InventoryWidget->HideItemInfoPopup();
		InventoryWidget->HideAllBorder();
		return;
	}

	SelectedInvenIndex = Index;

	if (SelectedInvenIndex != INDEX_NONE)
	{
		InventoryWidget->ShowBorder(SelectedInvenIndex);
		InventoryWidget->ShowItemInfoPopup(
			Items[SelectedInvenIndex].ItemName,
			Items[SelectedInvenIndex].ItemDescription
		);
	}
	else
	{
		InventoryWidget->HideAllBorder();
		InventoryWidget->HideItemInfoPopup();
	}

	AAudioManager* AudioManager =
	Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));

	UMyGameInstance* GI =
	Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	if (AudioManager && GI && GI->SFXData && GI->SFXData->PlayerChangeTool)
	{
		AudioManager->PlaySFX2D(GI->SFXData->PlayerChangeTool);
	}
	
}

void UInventoryComponent::UseSelectedItem()
{
	if (!Items.IsValidIndex(SelectedInvenIndex)) return;

	FInventoryItem& Item = Items[SelectedInvenIndex];

	if (Item.ItemType == EItemType::Wallet)
	{
		Item.ItemName = FText::FromString(TEXT("Card"));
		Item.Icon = CardIcon;
		Item.ItemType = EItemType::Card;

		UpdateInventoryUI();

		if (InventoryWidget)
		{
			InventoryWidget->ShowItemInfoPopup(Item.ItemName, Item.ItemDescription);
		}

		AAudioManager* AudioManager =
		Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));

		UMyGameInstance* GI =
			Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

		if (AudioManager && GI && GI->SFXData && GI->SFXData->PlayerPickupGain)
		{
			AudioManager->PlaySFX2D(GI->SFXData->PlayerPickupGain);
		}

		return; 
	}

	InventoryWidget->ShowEquippedItem(Item.Icon);
	EquippedItemType = Item.ItemType;
	OnEquipItemChanged.Broadcast(EquippedItemType);

	AAudioManager* AudioManager =
		Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));

	UMyGameInstance* GI =
	Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	if (AudioManager && GI && GI->SFXData && GI->SFXData->PlayerPickupGain)
	{
		AudioManager->PlaySFX2D(GI->SFXData->PlayerPickupGain);
	}
	
	UE_LOG(LogTemp, Log, TEXT("Equipped: %s"), *UEnum::GetValueAsString(EquippedItemType));
}


void UInventoryComponent::AddItem(AItemActor* ItemActor)
{
	if (!ItemActor) return;

	FInventoryItem NewItem;
	NewItem.Icon = ItemActor->ItemIcon;
	NewItem.ItemName = FText::FromName(ItemActor->ItemName);
	NewItem.ItemDescription = ItemActor->ItemDescription;
	NewItem.ItemType = ItemActor->ItemType;

	Items.Add(NewItem);
	InventoryWidget->HideItemInfoPopup();
	InventoryWidget->HideAllBorder();
	UpdateInventoryUI();
}

void UInventoryComponent::UpdateInventoryUI()
{
	if (InventoryWidget)
	{
		InventoryWidget->UpdateInventory(Items);
	}
}
