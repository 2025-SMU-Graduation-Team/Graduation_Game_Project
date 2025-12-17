	// Fill out your copyright notice in the Description page of Project Settings.


	#include "MyPaperCharacter.h"
	#include "EnhancedInputComponent.h"
	#include "EnhancedInputSubsystems.h"
	#include "PaperFlipbookComponent.h"
	#include "GameFramework/CharacterMovementComponent.h"
	#include "Components/CapsuleComponent.h"


	AMyPaperCharacter::AMyPaperCharacter()
	{
		PrimaryActorTick.bCanEverTick = true;

		GetCharacterMovement()->GravityScale = 2.0f;
		GetCharacterMovement()->AirControl = 0.8f;
		GetCharacterMovement()->MaxWalkSpeed = 400.0f;
		GetCharacterMovement()->JumpZVelocity = 800.0f;

		Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
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
		if (!bEnableMovement) return;

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

	void AMyPaperCharacter::Interact(const FInputActionValue& Value) 
	{
		if (CurrentInteractable)
		{
			CurrentInteractable->Interact();
		}
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
