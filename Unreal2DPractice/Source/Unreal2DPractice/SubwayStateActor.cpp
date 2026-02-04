#include "SubwayStateActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "LevelSequencePlayer.h"
#include "MyPaperCharacter.h"

ASubwayStateActor::ASubwayStateActor()
{
    CurrentState = ESubwayState::Approaching;
    ActiveCutscene = nullptr;
}

void ASubwayStateActor::BeginPlay()
{
    Super::BeginPlay();


    if (!FlowManager)
    {
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(
            this,
            AEndingFlowManager::StaticClass(),
            Found
        );

        if (Found.Num() > 0)
        {
            FlowManager = Cast<AEndingFlowManager>(Found[0]);
        }
    }
    SetState(CurrentState);
}

void ASubwayStateActor::SetState(ESubwayState NewState)
{
    CurrentState = NewState;

    const bool bEnable =
        (CurrentState == ESubwayState::DoorsOpen ||
            CurrentState == ESubwayState::Passed);

    UE_LOG(LogTemp, Log,
        TEXT("[SubwayStateActor] State: %s, Collider enabled: %s"),
        StateToString(CurrentState),
        bEnable ? TEXT("true") : TEXT("false"));

    if (TriggerBox)
    {
        TriggerBox->SetCollisionEnabled(
            bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision
        );
    }
}

void ASubwayStateActor::Interact()
{
    if (!CachedPlayer)
        return;

    if (CurrentState != ESubwayState::DoorsOpen &&
        CurrentState != ESubwayState::Passed)
        return;

    PlayEnterCutscene();
}

void ASubwayStateActor::PlayEnterCutscene()
{
    if (!CachedPlayer)
        return;

    if (CurrentState == ESubwayState::DoorsOpen)
    {
        ActiveCutscene = DoorsOpenCutscene;
    }
    else if (CurrentState == ESubwayState::Passed)
    {
        ActiveCutscene = PassedCutscene;
    }

    if (!ActiveCutscene)
        return;

    ULevelSequencePlayer* SeqPlayer = ActiveCutscene->GetSequencePlayer();
    if (!SeqPlayer)
        return;

    CachedPlayer->DisableInput(PC);

    ActiveCutscene->SetBindingByTag(
        TEXT("Player"),
        { CachedPlayer }
    );
    SeqPlayer->Play();
}

void ASubwayStateActor::HandleRide()
{
    if (CachedPlayer)
    {
        CachedPlayer->EnableInput(PC);
    }

    if (FlowManager)
    {
        FlowManager->RequestEnding(EEndingType::Normal);
    }
}

void ASubwayStateActor::HandleSkip()
{
    if (CachedPlayer)
    {
        UE_LOG(LogTemp, Log, TEXT("[SubwayStateActor] CachedPlayer is not null"));
    }
    if (CachedPlayer)
    {
        CachedPlayer->EnableInput(PC);
    }

    if (FlowManager)
    {
        FlowManager->RequestEnding(EEndingType::Hidden);
    }
}

const TCHAR* ASubwayStateActor::StateToString(ESubwayState State)
{
    switch (State)
    {
    case ESubwayState::Approaching: return TEXT("Approaching");
    case ESubwayState::DoorsOpen:   return TEXT("DoorsOpen");
    case ESubwayState::Leaving:     return TEXT("Leaving");
    case ESubwayState::Passed:      return TEXT("Passed");
    default:                        return TEXT("Unknown");
    }
}
