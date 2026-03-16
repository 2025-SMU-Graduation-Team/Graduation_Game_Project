#include "HiddenEndingReturnDispatcher.h"

#include "HiddenEndingReturnInterface.h"
#include "HiddenEndingStateSubsystem.h"

AHiddenEndingReturnDispatcher::AHiddenEndingReturnDispatcher()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AHiddenEndingReturnDispatcher::BeginPlay()
{
    Super::BeginPlay();

    if (!ShouldDispatch())
    {
        return;
    }

    ActivateManagedActors();
    NotifyManagedActors();
}

bool AHiddenEndingReturnDispatcher::ShouldDispatch() const
{
    UHiddenEndingStateSubsystem* HiddenEndingState =
        GetGameInstance() ? GetGameInstance()->GetSubsystem<UHiddenEndingStateSubsystem>() : nullptr;
    if (!HiddenEndingState)
    {
        return false;
    }

    if (TriggerMode == EHiddenEndingReturnTriggerMode::ImmediateReturn)
    {
        return HiddenEndingState->ConsumeImmediateReturn();
    }

    return HiddenEndingState->ConsumeFinalReturnForLevel(TargetLevelName);
}

void AHiddenEndingReturnDispatcher::ActivateManagedActors()
{
    for (AActor* Actor : ActorsToActivate)
    {
        if (!Actor)
        {
            continue;
        }

        Actor->SetActorHiddenInGame(false);
        Actor->SetActorEnableCollision(bEnableCollisionOnActivate);
        Actor->SetActorTickEnabled(bEnableTickOnActivate);
    }
}

void AHiddenEndingReturnDispatcher::NotifyManagedActors()
{
    for (AActor* Actor : ActorsToNotify)
    {
        if (!Actor)
        {
            continue;
        }

        if (Actor->GetClass()->ImplementsInterface(UHiddenEndingReturnInterface::StaticClass()))
        {
            IHiddenEndingReturnInterface::Execute_HandleHiddenEndingReturn(Actor);
        }
    }
}
