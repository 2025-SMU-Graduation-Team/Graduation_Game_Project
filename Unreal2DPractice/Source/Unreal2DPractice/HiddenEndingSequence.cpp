#include "HiddenEndingSequence.h"
#include "MyPaperCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "AudioManager.h"
#include "MyGameInstance.h"
#include "GameBGMData.h"

AHiddenEndingSequence::AHiddenEndingSequence()
{
    PrimaryActorTick.bCanEverTick = false;

    State = EHiddenEndingState::None;
}

void AHiddenEndingSequence::BeginPlay()
{
    Super::BeginPlay();
}

void AHiddenEndingSequence::StartSequence(AMyPaperCharacter* Player, FVector TeleportLocation)
{
    if (!Player)
        return;

    CachedPlayer = Player;
    PendingTeleportLocation = TeleportLocation;

    if (BackFlipbook)
    {
        CachedPlayer->SetForcedFlipbook(BackFlipbook);
    }

    StartMove();
}

void AHiddenEndingSequence::StartMove()
{
    State = EHiddenEndingState::Moving;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

    if (!CachedPlayer || !PC)
        return;

    CachedPlayer->DisableInput(PC);

    StartLocation = CachedPlayer->GetActorLocation();
    TargetLocation = StartLocation + FVector(0.f, 100.f, 0.f);

    MoveElapsed = 0.f;

    GetWorld()->GetTimerManager().SetTimer(
        MoveTimer,
        this,
        &AHiddenEndingSequence::UpdateMove,
        0.016f,
        true
    );
}

void AHiddenEndingSequence::UpdateMove()
{
    if (!CachedPlayer)
        return;

    MoveElapsed += 0.016f;

    float Alpha = FMath::Clamp(MoveElapsed / MoveDuration, 0.f, 1.f);
    Alpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.f);

    FVector NewLoc = FMath::Lerp(StartLocation, TargetLocation, Alpha);
    CachedPlayer->SetActorLocation(NewLoc);

    if (Alpha >= 1.f)
    {
        GetWorld()->GetTimerManager().ClearTimer(MoveTimer);
        FinishSequence();
    }
}

void AHiddenEndingSequence::FinishSequence()
{
    State = EHiddenEndingState::FadingOut;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

    if (!PC || !PC->PlayerCameraManager)
        return;

    PC->PlayerCameraManager->StartCameraFade(
        0.f, 1.f, 0.25f, FLinearColor::Black, false, true
    );

    FTimerHandle FadeHandle;
    GetWorld()->GetTimerManager().SetTimer(FadeHandle, [this, PC]()
        {
            if (!CachedPlayer)
                return;

            CachedPlayer->ClearForcedFlipbook();
            UGameplayStatics::OpenLevel(this, DestinationLevelName);

        }, 0.25f, false);

    if (CachedPlayer && PC)
    {
        CachedPlayer->EnableInput(PC);
    }
}
