
#include "SubwayStateActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "MyPaperCharacter.h"

ASubwayStateActor::ASubwayStateActor()
{
    CurrentState = ESubwayState::Approaching;
}

void ASubwayStateActor::BeginPlay()
{
    Super::BeginPlay();
    SetState(CurrentState);
}

void ASubwayStateActor::SetState(ESubwayState NewState)
{
    CurrentState = NewState;

    const bool bEnable = (CurrentState == ESubwayState::DoorsOpen || CurrentState == ESubwayState::Passed);

    UE_LOG(
        LogTemp,
        Log,
        TEXT("[SubwayStateActor] State: %s, Collider enabled: %s"),
        StateToString(CurrentState),
        bEnable ? TEXT("true") : TEXT("false")
    );

    TriggerBox->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ASubwayStateActor::Interact()
{
    if (!CachedPlayer)
        return;

    switch (CurrentState)
    {
    case ESubwayState::DoorsOpen:
        HandleRide();
        break;

    case ESubwayState::Passed:
        HandleSkip();
        break;

    default:
        break;
    }
}

void ASubwayStateActor::HandleRide()
{
    UGameplayStatics::LoadStreamLevel(this, "NormalEnding", true, false, FLatentActionInfo());
    if (CachedPlayer)
    {
        CachedPlayer->SetActorLocation(NormalTeleportLocation);
    }
}

void ASubwayStateActor::HandleSkip()
{
    UGameplayStatics::LoadStreamLevel(this, "HiddenEnding", true, false, FLatentActionInfo());
    if (CachedPlayer)
    {
        CachedPlayer->SetActorLocation(HiddenTeleportLocation);
    }
}

const TCHAR* ASubwayStateActor::StateToString(ESubwayState State)
{
    {
        switch (State)
        {
            case ESubwayState::Approaching:  return TEXT("Approaching");
            case ESubwayState::DoorsOpen: return TEXT("DoorsOpen");
            case ESubwayState::Leaving:      return TEXT("Leaving");
            case ESubwayState::Passed:    return TEXT("Passed");
            default:                      return TEXT("Unknown");
        }
    }
}
