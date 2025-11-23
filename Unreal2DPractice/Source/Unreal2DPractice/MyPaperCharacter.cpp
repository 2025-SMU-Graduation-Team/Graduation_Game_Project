// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPaperCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interactable.h"
#include "Components/CapsuleComponent.h"

AMyPaperCharacter::AMyPaperCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // Active Tick Function

	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.8f;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;
}

void AMyPaperCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyPaperCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AMyPaperCharacter::OnOverlapEnd);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(InputMappingContext, 0);
		}

		if (InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UItemInventoryWidget>(PC, InventoryWidgetClass);
			if (InventoryWidget)
			{
				InventoryWidget->AddToViewport();
			}
		}
	}
}

void AMyPaperCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPaperCharacter::Move);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyPaperCharacter::StartJump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyPaperCharacter::StopJump);
		EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &AMyPaperCharacter::Interact);
	}
}

void AMyPaperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAnimation();
}

void AMyPaperCharacter::Move(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisValue);
	UpdateCharacterDirection(AxisValue);
	
}

void AMyPaperCharacter::StartJump(const FInputActionValue& Value)
{
	Jump();
}

void AMyPaperCharacter::StopJump(const FInputActionValue& Value)
{
	StopJumping();
}

void AMyPaperCharacter::UpdateCharacterDirection(float AxisValue)
{
	if (AxisValue > 0)
	{
		GetSprite()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
	else if (AxisValue < 0)
	{
		GetSprite()->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	}
}

void AMyPaperCharacter::UpdateAnimation()
{
	if (bIsDead) return;

	UPaperFlipbook* DesiredAnimation = IdleAnimation;
	if (GetCharacterMovement()->IsFalling())
	{
		DesiredAnimation = JumpAnimation;
	}
	else if (FMath::Abs(GetVelocity().X) > 0.1f)
	{
		DesiredAnimation = RunAnimation;
	}

	if (GetSprite()->GetFlipbook() != DesiredAnimation)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void AMyPaperCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin with %s"), *OtherActor->GetName());
	if (OtherActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		CurrentInteractable = OtherActor;
	}
}

void AMyPaperCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == CurrentInteractable)
	{
		CurrentInteractable = nullptr;
	}
}

void AMyPaperCharacter::PlayDeath()
{
	bIsDead = true;

	/*APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->DisableInput(PC);
	}*/

	if (DieAnimation)
	{
		GetSprite()->SetFlipbook(DieAnimation);
	}

	/*GetCharacterMovement()->DisableMovement();


	FTimerHandle DeathTimer;
	GetWorld()->GetTimerManager().SetTimer(
		DeathTimer,
		this,
		&AMyPaperCharacter::K2_DestroyActor,
		1.2f,    
		false
	);*/
}

void AMyPaperCharacter::RequestItemPickup(AItemActor* Item)
{
	if (!Item) return;

	PendingItem = Item;

	if (InventoryWidget)
	{
		InventoryWidget->ShowConfirmPopup(Item->ItemIcon);
	}
}

void AMyPaperCharacter::ConfirmPickupYes()
{
	if (!PendingItem) return;

	AddItem(PendingItem->ItemIcon);
	PendingItem->Destroy();
	PendingItem = nullptr;

	if (InventoryWidget)
	{
		InventoryWidget->HideConfirmPopup();
	}
}

void AMyPaperCharacter::ConfirmPickupNo()
{
	PendingItem = nullptr;

	if (InventoryWidget)
	{
		InventoryWidget->HideConfirmPopup();
	}
}

void AMyPaperCharacter::AddItem(UTexture2D* Item)
{
	if (Item == nullptr) return;

	InventoryItems.Add(Item);
	UE_LOG(LogTemp, Log, TEXT("Add Item"));
	UpdateInventoryUI();
}

void AMyPaperCharacter::UpdateInventoryUI()
{
	if (InventoryWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("UpdateInventoryUI"));
		InventoryWidget->UpdateInventory(InventoryItems);
	}
}

void AMyPaperCharacter::Interact()
{
	if (CurrentInteractable && CurrentInteractable->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		IInteractable::Execute_Interact(CurrentInteractable);
	}
}
