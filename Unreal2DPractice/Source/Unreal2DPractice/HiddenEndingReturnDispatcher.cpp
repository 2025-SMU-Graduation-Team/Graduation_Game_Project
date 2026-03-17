#include "HiddenEndingReturnDispatcher.h"

#include "HiddenEndingReturnInterface.h"
#include "HiddenEndingStateSubsystem.h"
#include "OpeningDoorInterface.h"

AHiddenEndingReturnDispatcher::AHiddenEndingReturnDispatcher()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AHiddenEndingReturnDispatcher::BeginPlay()
{
    Super::BeginPlay();

    const bool bShouldDispatch = ShouldDispatch();
    UE_LOG(
        LogTemp,
        Log,
        TEXT("[HiddenEndingReturnDispatcher] BeginPlay on %s TriggerMode=%d TargetLevel=%s ShouldDispatch=%s"),
        *GetName(),
        static_cast<int32>(TriggerMode),
        *TargetLevelName.ToString(),
        bShouldDispatch ? TEXT("true") : TEXT("false")
    );

    if (!bShouldDispatch)
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

        UE_LOG(
            LogTemp,
            Log,
            TEXT("[HiddenEndingReturnDispatcher] Activating actor %s from dispatcher %s"),
            *Actor->GetName(),
            *GetName()
        );

        Actor->SetActorHiddenInGame(false);
        Actor->SetActorEnableCollision(bEnableCollisionOnActivate);
        Actor->SetActorTickEnabled(bEnableTickOnActivate);

        if (Actor->GetClass()->ImplementsInterface(UOpeningDoorInterface::StaticClass()))
        {
            UE_LOG(
                LogTemp,
                Log,
                TEXT("[HiddenEndingReturnDispatcher] Restoring door state for %s"),
                *Actor->GetName()
            );
            IOpeningDoorInterface::Execute_RestoreOpenedState(Actor);
        }
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

        UE_LOG(
            LogTemp,
            Log,
            TEXT("[HiddenEndingReturnDispatcher] Notifying actor %s from dispatcher %s"),
            *Actor->GetName(),
            *GetName()
        );

        if (Actor->GetClass()->ImplementsInterface(UHiddenEndingReturnInterface::StaticClass()))
        {
            IHiddenEndingReturnInterface::Execute_HandleHiddenEndingReturn(Actor);
        }
    }
}
