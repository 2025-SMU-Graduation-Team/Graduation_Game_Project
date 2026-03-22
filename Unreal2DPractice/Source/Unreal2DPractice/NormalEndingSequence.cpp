#include "NormalEndingSequence.h"
#include "MyPaperCharacter.h"
#include "NormalEndingWidget.h"
#include "PaperFlipbook.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "TimerManager.h"

ANormalEndingSequence::ANormalEndingSequence()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ANormalEndingSequence::BeginPlay()
{
    Super::BeginPlay();
}

void ANormalEndingSequence::StartSequence(AMyPaperCharacter* Player, FVector TeleportLocation)
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

void ANormalEndingSequence::StartMove()
{
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
        &ANormalEndingSequence::UpdateMove,
        0.016f,
        true
    );
}

void ANormalEndingSequence::UpdateMove()
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
        OnMoveFinished();
    }
}

void ANormalEndingSequence::OnMoveFinished()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC || !PC->PlayerCameraManager)
        return;

    PC->PlayerCameraManager->StartCameraFade(
        0.f, 1.f, 0.3f, FLinearColor::Black, false, true
    );

    FTimerHandle FadeHandle;

    GetWorld()->GetTimerManager().SetTimer(FadeHandle, [this, PC]()
        {
            if (!CachedPlayer)
                return;

            CachedPlayer->ClearForcedFlipbook();
            UGameplayStatics::OpenLevel(this, DestinationLevelName);

        }, 0.3f, false);
}

void ANormalEndingSequence::ShowEndingWidget()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
        return;

    if (EndingWidgetClass)
    {
        ActiveWidget = CreateWidget<UNormalEndingWidget>(PC, EndingWidgetClass);
        if (ActiveWidget)
        {
            ActiveWidget->AddToViewport();
            ActiveWidget->StartShake(5.f);
        }
    }

    FTimerHandle EndHandle;

    GetWorld()->GetTimerManager().SetTimer(
        EndHandle,
        this,
        &ANormalEndingSequence::FinishSequence,
        5.f,
        false
    );
}

void ANormalEndingSequence::FinishSequence()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC || !PC->PlayerCameraManager)
        return;

    if (ActiveWidget)
    {
        ActiveWidget->RemoveFromParent();
    }

    PC->PlayerCameraManager->StartCameraFade(
        0.f, 1.f, 0.3f, FLinearColor::Black, false, true
    );

    FTimerHandle FinalHandle;

    GetWorld()->GetTimerManager().SetTimer(FinalHandle, [PC]()
        {
            if (PC && PC->PlayerCameraManager)
            {
                PC->PlayerCameraManager->StartCameraFade(
                    1.f, 0.f, 0.4f, FLinearColor::Black, false, false
                );
            }

        }, 0.3f, false);
}
