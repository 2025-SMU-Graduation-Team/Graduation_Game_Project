// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPaperCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "HidingSpot.h"

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
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
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
	if (bIsHidden)
	{
		ExitHide();
		return;
	}
	if (bCanHide && CurrentHidingSpot)
	{
		EnterHide();
		return;
	}
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

void AMyPaperCharacter::SetCanHide(AHidingSpot* Spot)
{
	if (!Spot) return;

	bCanHide = true;
	CurrentHidingSpot = Spot;

	UE_LOG(LogTemp, Log, TEXT("Can hide at: %s"), *Spot->GetName());
}

void AMyPaperCharacter::ClearCanHide(AHidingSpot* Spot)
{
	if (CurrentHidingSpot == Spot)
	{
		bCanHide = false;
		CurrentHidingSpot = nullptr;

		// 숨은 상태로 범위를 벗어났다면 자동으로 나오도록
		if (bIsHidden)
		{
			ExitHide();
		}

		UE_LOG(LogTemp, Log, TEXT("Left hiding spot"));
	}
}

void AMyPaperCharacter::EnterHide()
{
	if (!bCanHide || !CurrentHidingSpot)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnterHide: No available hiding spot."));
		return;
	}

	bIsHidden = true;

	// 1) 위치를 HidingSpot 근처로 붙이기
	FVector SpotLocation = CurrentHidingSpot->GetActorLocation();
	FVector NewLocation = GetActorLocation();

	// 2D 사이드뷰 기준: X만 맞춰서 물체 뒤로 살짝 숨는 느낌
	NewLocation.X = SpotLocation.X;
	SetActorLocation(NewLocation);

	// 2) 이동 막기 (숨은 동안 플레이어가 움직이지 않게)
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->DisableMovement();
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

	// 이동 다시 허용
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	UE_LOG(LogTemp, Log, TEXT("Player exited hiding"));
}
