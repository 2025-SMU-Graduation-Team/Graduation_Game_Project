#include "SubwayStateActor.h"
#include "EndingDirector.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ASubwayStateActor::ASubwayStateActor()
{
    CurrentState = ESubwayState::Approaching;
}

void ASubwayStateActor::BeginPlay()
{
    Super::BeginPlay();

    if (!EndingDirector)
    {
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(
            this,
            AEndingDirector::StaticClass(),
            Found
        );

        if (Found.Num() > 0)
        {
            EndingDirector = Cast<AEndingDirector>(Found[0]);
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

    if (TriggerBox)
    {
        TriggerBox->SetCollisionEnabled(
            bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision
        );
    }
}

void ASubwayStateActor::Interact()
{
    if (!CachedPlayer || !EndingDirector)
        return;

    UE_LOG(LogTemp, Log, TEXT("[Interact] CurrentState %s"), StateToString(CurrentState));
    if (CurrentState == ESubwayState::DoorsOpen)
    {
        EndingDirector->StartNormalEnding(
            CachedPlayer,
            NormalTeleportLocation
        );
    }
    else if (CurrentState == ESubwayState::Passed)
    {
        EndingDirector->StartHiddenEnding(
            CachedPlayer,
            HiddenTeleportLocation
        );
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
