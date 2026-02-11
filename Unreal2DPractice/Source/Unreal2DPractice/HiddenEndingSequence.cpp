#include "HiddenEndingSequence.h"
#include "MyPaperCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "LevelTransitionManager.h"

AHiddenEndingSequence::AHiddenEndingSequence()
{
    PrimaryActorTick.bCanEverTick = true;

    State = EHiddenEndingState::None;
    MoveSpeed = 200.f;
}

void AHiddenEndingSequence::BeginPlay()
{
    Super::BeginPlay();
}

void AHiddenEndingSequence::StartSequence(AMyPaperCharacter* Player, FVector TeleportLocation)
{
    if (!Player)
        return;

    UE_LOG(LogTemp, Log, TEXT("HiddenEnding StartSequence"));
    CachedPlayer = Player;
    PendingTeleportLocation = TeleportLocation;

    State = EHiddenEndingState::WaitingInput;
}

void AHiddenEndingSequence::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (State == EHiddenEndingState::WaitingInput)
    {
        CheckInput();
    }
    else if (State == EHiddenEndingState::Moving)
    {
        UpdateMove(DeltaTime);
    }
}

void AHiddenEndingSequence::CheckInput()
{
    APlayerController* PC =
        UGameplayStatics::GetPlayerController(this, 0);

    if (!PC)
        return;

    if (PC->IsInputKeyDown(EKeys::W))
    {
        UE_LOG(LogTemp, Log, TEXT("[HiddenEnding] CheckInput"));
        StartMove();
    }
}

void AHiddenEndingSequence::StartMove()
{
    UE_LOG(LogTemp, Log, TEXT("[HiddenEnding] StartMove"));

    State = EHiddenEndingState::Moving;

    APlayerController* PC =
        UGameplayStatics::GetPlayerController(this, 0);

    if (CachedPlayer && PC)
    {
        CachedPlayer->DisableInput(PC);

        if (BackMontage)
        {
            CachedPlayer->PlayAnimMontage(BackMontage);
        }

        StartLocation = CachedPlayer->GetActorLocation();
        TargetLocation = StartLocation + FVector(0.f, 70.f, 0.f);

        MoveElapsed = 0.f;
    }
}

void AHiddenEndingSequence::UpdateMove(float DeltaTime)
{
    MoveElapsed += DeltaTime;

    float Alpha = FMath::Clamp(MoveElapsed / MoveDuration, 0.f, 1.f);

    Alpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.f);

    FVector NewLoc = FMath::Lerp(StartLocation, TargetLocation, Alpha);

    CachedPlayer->SetActorLocation(NewLoc);

    if (Alpha >= 1.f)
    {
        FinishSequence();
    }
}

void AHiddenEndingSequence::FinishSequence()
{
    State = EHiddenEndingState::FadingOut;

    APlayerController* PC =
        UGameplayStatics::GetPlayerController(this, 0);

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

            CachedPlayer->SetActorLocation(PendingTeleportLocation);

            ALevelTransitionManager* Manager =
                ALevelTransitionManager::Get(GetWorld());

            if (Manager)
            {
                Manager->ChangeSubLevel(TEXT("HiddenEnding"));
            }

            FTimerHandle ShowHandle;
            GetWorld()->GetTimerManager().SetTimer(ShowHandle, [PC]()
                {
                    if (PC && PC->PlayerCameraManager)
                    {
                        PC->PlayerCameraManager->StartCameraFade(
                            1.f, 0.f, 0.4f, FLinearColor::Black, false, false
                        );
                    }

                }, 0.3f, false);

        }, 0.25f, false);

    if (CachedPlayer && PC)
    {
        CachedPlayer->EnableInput(PC);
    }
}
