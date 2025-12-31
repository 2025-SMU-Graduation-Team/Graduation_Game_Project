
#include "SubwayStateActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

ASubwayStateActor::ASubwayStateActor()
{
    CurrentState = ESubwayState::Approaching;
}

void ASubwayStateActor::BeginPlay()
{
    Super::BeginPlay();
}

void ASubwayStateActor::SetState(ESubwayState NewState)
{
    CurrentState = NewState;

    const bool bEnable = (CurrentState == ESubwayState::DoorsOpen || CurrentState == ESubwayState::Passed);

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
}

void ASubwayStateActor::HandleSkip()
{
    UGameplayStatics::LoadStreamLevel(this, "HiddenEnding", true, false, FLatentActionInfo());
}
