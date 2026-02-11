#include "NormalEndingSequence.h"
#include "MyPaperCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "TimerManager.h"
#include "SubLevelTaskManager.h"

ANormalEndingSequence::ANormalEndingSequence()
{
    PrimaryActorTick.bCanEverTick = true;

    State = ENormalEndingState::None;
    MoveSpeed = 200.f;

    ShakeDuration = 3.f;
    ShakeAmplitude = 5.f;
    ShakeSpeed = 5.f;
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

    CachedPlayer->DisableInput(
        UGameplayStatics::GetPlayerController(this, 0)
    );

    StartLocation = CachedPlayer->GetActorLocation();
    TargetLocation = StartLocation + FVector(0.f, -100.f, 0.f);

    State = ENormalEndingState::Moving;
}

void ANormalEndingSequence::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (State == ENormalEndingState::Moving)
    {
        UpdateMove(DeltaTime);
    }
    else if (State == ENormalEndingState::ShowingImage)
    {
        UpdateCameraShake(DeltaTime);
    }
}

void ANormalEndingSequence::UpdateMove(float DeltaTime)
{
    FVector Current = CachedPlayer->GetActorLocation();
    FVector NewLoc = FMath::VInterpConstantTo(Current, TargetLocation, DeltaTime, MoveSpeed);

    CachedPlayer->SetActorLocation(NewLoc);

    if (FVector::Dist(NewLoc, TargetLocation) < 1.f)
    {
        CloseDoor();
    }
}

void ANormalEndingSequence::CloseDoor()
{
    if (TaskManager && DoorActor)
    {
        TaskManager->CloseDoor(DoorActor);
    }

    StartImagePhase();
}

void ANormalEndingSequence::StartImagePhase()
{
    State = ENormalEndingState::ShowingImage;
    ShakeTime = 0.f;

    // show EndingImage
}

void ANormalEndingSequence::UpdateCameraShake(float DeltaTime)
{
    ShakeTime += DeltaTime;

    APlayerCameraManager* Cam =
        UGameplayStatics::GetPlayerCameraManager(this, 0);

    if (!Cam)
        return;

    float Offset = FMath::Sin(ShakeTime * ShakeSpeed) * ShakeAmplitude;

    FRotator Rot = Cam->GetCameraRotation();
    Rot.Pitch += Offset * DeltaTime;

    Cam->SetManualCameraFade(0.f, FLinearColor::Black, false);

    if (ShakeTime >= ShakeDuration)
    {
        FinishSequence();
    }
}

void ANormalEndingSequence::FinishSequence()
{
    State = ENormalEndingState::FadingOut;

    APlayerController* PC =
        UGameplayStatics::GetPlayerController(this, 0);

    PC->PlayerCameraManager->StartCameraFade(
        0.f, 1.f, 1.f, FLinearColor::Black, false, true
    );
    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, [this, PC]()
    {
            UGameplayStatics::OpenLevel(this, "NormalEndingLevel");

            if (CachedPlayer)
            {
                CachedPlayer->SetActorLocation(PendingTeleportLocation);
            }

            PC->PlayerCameraManager->StartCameraFade(
                0.f, 1.f, 1.f, FLinearColor::Black, false, true
            );
    }, 1.f, false);
}
