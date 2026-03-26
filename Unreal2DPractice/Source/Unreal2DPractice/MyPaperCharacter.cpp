// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPaperCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCameraController.h"
#include "CameraLimitVolume.h"
#include "HidingSpot.h"
#include "AudioManager.h"
#include "MyGameInstance.h"
#include "GameSFXData.h"


AMyPaperCharacter::AMyPaperCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.8f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	GetCharacterMovement()->SetPlaneConstraintEnabled(true);
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);

	GetCharacterMovement()->SetWalkableFloorAngle(55.0f);
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->MaxStepHeight = 5.0f;
	GetCharacterMovement()->PerchRadiusThreshold = 0.0f;

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	CameraController = CreateDefaultSubobject<UPlayerCameraController>(TEXT("CameraController"));
	WalkAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WalkAudioComponent"));
	WalkAudioComponent->SetupAttachment(RootComponent);
	WalkAudioComponent->bAutoActivate = false;
}

void AMyPaperCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(
		this,
		&AMyPaperCharacter::OnOverlapBegin
	);

	GetCapsuleComponent()->UpdateOverlaps();
	bPendingInitialCameraLimitResolve = !ResolveInitialCameraLimit();

	DefaultSpriteOffset = GetSprite()->GetRelativeLocation();

	if (Inventory)
	{
		Inventory->OnEquipItemChanged.RemoveAll(this); 

		/*Inventory->OnEquipItemChanged.AddDynamic(
			this,
			&AMyPaperCharacter::OnItemChanged
		);*/
	}
}

void AMyPaperCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult
)
{
	ACameraLimitVolume* Volume = Cast<ACameraLimitVolume>(OtherActor);

	if (Volume && CameraController)
	{
		CameraController->SetLimitVolume(Volume);
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
		EnhancedInput->BindAction(SelectSlotAction, ETriggerEvent::Triggered, this, &AMyPaperCharacter::OnSelectSlot);
		EnhancedInput->BindAction(UseItemAction, ETriggerEvent::Triggered, this, &AMyPaperCharacter::OnUseItem);
		EnhancedInput->BindAction(EnterCutSceneAction, ETriggerEvent::Triggered, this, &AMyPaperCharacter::EnterCutScene);

		EnhancedInput->BindAction(HideAction, ETriggerEvent::Started, this, &AMyPaperCharacter::OnHidePressed);
		EnhancedInput->BindAction(HideAction, ETriggerEvent::Completed, this, &AMyPaperCharacter::OnHideReleased);
		EnhancedInput->BindAction(HideAction, ETriggerEvent::Canceled, this, &AMyPaperCharacter::OnHideReleased);
	}
	UE_LOG(LogTemp, Warning, TEXT("[INPUT] Setup bindings done. HideAction=%s"), *GetNameSafe(HideAction));
}

void AMyPaperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPendingInitialCameraLimitResolve)
	{
		bPendingInitialCameraLimitResolve = !ResolveInitialCameraLimit();
	}

	UpdateAnimation();
	UpdateWalkAudio();
}

void AMyPaperCharacter::Move(const FInputActionValue& Value)
{
	if (!bEnableMovement) return;

	float AxisValue = Value.Get<float>();
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisValue);
	UpdateCharacterDirection(AxisValue);
	
}

bool AMyPaperCharacter::CanJumpInternal_Implementation() const
{
	if(!Super::CanJumpInternal_Implementation()) return false;

	const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp) return false;

	if (!MoveComp->IsMovingOnGround())
		return true;

	const FVector floorNormal = MoveComp->CurrentFloor.HitResult.ImpactNormal;
	float slope = FVector::DotProduct(floorNormal, FVector::UpVector);

	if (slope < 0.75f)
	{
		return false;
	}

	return true;
}

void AMyPaperCharacter::StartJump(const FInputActionValue& Value)
{
	if (!CanJump())
		return;

	Jump();

	AAudioManager* AudioManager =
		Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));

	UMyGameInstance* GI =
		Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	if (AudioManager && GI && GI->SFXData && GI->SFXData->PlayerJumpLanding)
	{
		AudioManager->PlaySFX2D(GI->SFXData->PlayerJumpLanding);
	}
}

void AMyPaperCharacter::StopJump(const FInputActionValue& Value)
{
	StopJumping();
}

void AMyPaperCharacter::Interact(const FInputActionValue& Value) 
{
	if (CurrentInteractable)
	{
		CurrentInteractable->Interact();
	}
}

void AMyPaperCharacter::EnterCutScene(const FInputActionValue& Value)
{
	if (CurrentSubway)
	{
		UE_LOG(LogTemp, Log, TEXT("CurrentSubway is vaild"));
		CurrentSubway->Interact(this);
		return;
	}

	if (TryInteractFromEnterKey())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("W key is valid but no enter-key interaction target is available"));
}

void AMyPaperCharacter::OnSelectSlot(const FInputActionValue& Value)
{
	if (!Inventory) return;

	int32 SlotIndex = FMath::RoundToInt(Value.Get<float>());
	UE_LOG(LogTemp, Log, TEXT("Key Pressed Slot(Index): %d"), SlotIndex);

	Inventory->SelectSlot(SlotIndex - 1);
}

void AMyPaperCharacter::OnUseItem(const FInputActionValue& Value)
{
	if (!Inventory) return;

	UE_LOG(LogTemp, Log, TEXT("Use Item by Pressing Q"));
	Inventory->UseSelectedItem();
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

	if (ForcedFlipbook)
	{
		if (GetSprite()->GetFlipbook() != ForcedFlipbook)
		{
			GetSprite()->SetFlipbook(ForcedFlipbook);
		}
		return;
	}

	if (bIsHidden)
	{
		if (HideAnimation && GetSprite()->GetFlipbook() != HideAnimation)
		{
			GetSprite()->SetFlipbook(HideAnimation);
		}
		return;
	}

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

void AMyPaperCharacter::PlayDeath()
{
	if (bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Death] PlayDeath blocked because bIsDead is already true"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Death] PlayDeath called"));

	bIsDead = true;
	bEnableMovement = false;
	StopWalkLoop();

	AAudioManager* AudioManager =
		Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));

	UMyGameInstance* GI =
		Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	if (AudioManager && GI && GI->SFXData)
	{
		USoundBase* DeathSound = nullptr;

		if (GI->LastDeathCause == EDeathCause::NPC)
		{
			DeathSound = GI->SFXData->PlayerDeathNPC;
		}
		else
		{
			DeathSound = GI->SFXData->PlayerDeathScream;
		}

		if (DeathSound)
		{
			AudioManager->PlaySFX2D(DeathSound);
		}
	}

	if (DieAnimation)
	{
		GetSprite()->SetFlipbook(DieAnimation);
	}

	GetCharacterMovement()->DisableMovement();

	FTimerHandle DeathTimer;
	GetWorld()->GetTimerManager().SetTimer(
		DeathTimer,
		this,
		&AMyPaperCharacter::GoToGameOverLevel,
		1.2f,    
		false
	);
}

void AMyPaperCharacter::GoToGameOverLevel()
{
	UE_LOG(LogTemp, Warning, TEXT("[Death] GoToGameOverLevel called"));
	UGameplayStatics::OpenLevel(this, FName("GameOver"));
}

// =====================
//   Hide 
// =====================
void AMyPaperCharacter::SetCanHide(AHidingSpot* Spot)
{
	if (!Spot) return;

	bCanHide = true;
	CurrentHidingSpot = Spot;

	UE_LOG(LogTemp, Warning, TEXT("[HIDE] Can hide at: %s"), *GetNameSafe(Spot));
}

void AMyPaperCharacter::ClearCanHide(AHidingSpot* Spot)
{
	if (CurrentHidingSpot != Spot) return;

	UE_LOG(LogTemp, Warning, TEXT("[HIDE] ClearCanHide: %s"), *GetNameSafe(Spot));

	bCanHide = false;
	CurrentHidingSpot = nullptr;

	if (bIsHidden)
	{
		ExitHide();
	}
}

void AMyPaperCharacter::EnterHide()
{
	UE_LOG(LogTemp, Warning, TEXT("[HIDE] EnterHide called. bCanHide=%d Spot=%s"),
		bCanHide, *GetNameSafe(CurrentHidingSpot));

	if (!bCanHide || !CurrentHidingSpot)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnterHide: No available hiding spot."));
		return;
	}

	bIsHidden = true;
	StopWalkLoop();

	AAudioManager* AudioManager =
		Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(),
		 AAudioManager::StaticClass()));

	UMyGameInstance* GI =
		Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	if (AudioManager && GI && GI->SFXData && GI->SFXData->PlayerHide)
	{
		AudioManager->PlaySFX2D(GI->SFXData->PlayerHide);
	}

	UE_LOG(LogTemp, Log, TEXT("Player is now hiding."));
}

void AMyPaperCharacter::ExitHide()
{
	if (!bIsHidden)
	{
		return;
	}

	bIsHidden = false;
	GetSprite()->SetRelativeLocation(DefaultSpriteOffset);

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	UE_LOG(LogTemp, Log, TEXT("Player exited hiding"));
}

void AMyPaperCharacter::OnHidePressed(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("[HIDE] OnHidePressed fired")); 
	if (bIsDead) return;

	UE_LOG(LogTemp, Warning, TEXT("[HIDE] Pressed. bCanHide=%d bIsHidden=%d Spot=%s"),
		bCanHide, bIsHidden, *GetNameSafe(CurrentHidingSpot));

	if (bCanHide)
	{
		EnterHide();
	}
}
void AMyPaperCharacter::OnHideReleased(const FInputActionValue& Value)
{
	if (bIsDead) return;

	if (!bIsHidden) return;

	ExitHide();
}

void AMyPaperCharacter::SetCurrentSubway(ASubwayStateActor* Subway)
{
	CurrentSubway = Subway;
}

void AMyPaperCharacter::SetForcedFlipbook(UPaperFlipbook* NewFlipbook)
{
	ForcedFlipbook = NewFlipbook;
}

void AMyPaperCharacter::ClearForcedFlipbook()
{
	ForcedFlipbook = nullptr;
}

void AMyPaperCharacter::RefreshAfterLevelTransition()
{
	if (CameraController)
	{
		CameraController->SetLimitVolume(nullptr);
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->UpdateOverlaps();
	}

	bPendingInitialCameraLimitResolve = !ResolveInitialCameraLimit();
}

bool AMyPaperCharacter::TryInteractFromEnterKey()
{
	if (!CurrentInteractable || !CurrentInteractable->CanInteractFromEnterKey())
	{
		return false;
	}

	CurrentInteractable->InteractFromEnterKey();
	return true;
}

void AMyPaperCharacter::UpdateWalkAudio()
{
	if (!WalkAudioComponent)
	{
		return;
	}

	const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	const bool bShouldPlayWalkLoop =
		bEnableMovement &&
		!bIsDead &&
		!bIsHidden &&
		MoveComp &&
		MoveComp->IsMovingOnGround() &&
		FMath::Abs(GetVelocity().X) > 10.0f;

	if (bShouldPlayWalkLoop)
	{
		StartWalkLoop();
	}
	else
	{
		StopWalkLoop();
	}
}

void AMyPaperCharacter::StartWalkLoop()
{
	if (!WalkAudioComponent || WalkAudioComponent->IsPlaying())
	{
		return;
	}

	UMyGameInstance* GI =
		Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	if (!GI || !GI->SFXData || !GI->SFXData->PlayerWalk)
	{
		return;
	}

	WalkAudioComponent->SetSound(GI->SFXData->PlayerWalk);
	WalkAudioComponent->Play();
}

void AMyPaperCharacter::StopWalkLoop()
{
	if (!WalkAudioComponent || !WalkAudioComponent->IsPlaying())
	{
		return;
	}

	WalkAudioComponent->Stop();
}

bool AMyPaperCharacter::ResolveInitialCameraLimit()
{
	if (!CameraController)
	{
		return false;
	}

	TArray<AActor*> CameraLimitVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraLimitVolume::StaticClass(), CameraLimitVolumes);

	const FVector PlayerLocation = GetActorLocation();

	for (AActor* CameraLimitActor : CameraLimitVolumes)
	{
		ACameraLimitVolume* Volume = Cast<ACameraLimitVolume>(CameraLimitActor);
		if (!Volume)
		{
			continue;
		}

		if (Volume->ContainsWorldLocation(PlayerLocation))
		{
			CameraController->SetLimitVolume(Volume);
			return true;
		}
	}

	return false;
}
