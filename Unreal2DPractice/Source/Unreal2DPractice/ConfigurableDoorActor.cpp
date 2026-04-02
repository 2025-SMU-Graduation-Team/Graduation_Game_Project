#include "ConfigurableDoorActor.h"

#include "Components/SceneComponent.h"

AConfigurableDoorActor::AConfigurableDoorActor()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void AConfigurableDoorActor::BeginPlay()
{
    Super::BeginPlay();

    CacheDoorComponents();

    if (bStartOpened)
    {
        ApplyOpenTransform();
        return;
    }

    ApplyClosedTransform();
}

void AConfigurableDoorActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsTransitioning)
    {
        return;
    }

    if (TransitionDuration <= KINDA_SMALL_NUMBER)
    {
        FinishTransition(TransitionTargetAlpha);
        return;
    }

    TransitionElapsed += DeltaTime;

    float Alpha = FMath::Clamp(TransitionElapsed / TransitionDuration, 0.f, 1.f);
    if (bUseSmoothTransition)
    {
        Alpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, InterpExponent);
    }

    const float BlendedAlpha = FMath::Lerp(TransitionStartAlpha, TransitionTargetAlpha, Alpha);
    ApplyAlpha(BlendedAlpha);

    if (Alpha >= 1.f)
    {
        FinishTransition(TransitionTargetAlpha);
    }
}

void AConfigurableDoorActor::OpenDoor_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("[ConfigurableDoorActor] OpenDoor called on %s"), *GetName());
    TransitionTargetAlpha = 1.f;
    StartTransition();
}

void AConfigurableDoorActor::CloseDoor_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("[ConfigurableDoorActor] CloseDoor called on %s"), *GetName());
    TransitionTargetAlpha = 0.f;
    StartTransition();
}

void AConfigurableDoorActor::RestoreOpenedState_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("[ConfigurableDoorActor] RestoreOpenedState called on %s"), *GetName());
    ApplyOpenTransform();
}

void AConfigurableDoorActor::ApplyClosedTransform()
{
    FinishTransition(0.f);
}

void AConfigurableDoorActor::ApplyOpenTransform()
{
    FinishTransition(1.f);
}

USceneComponent* AConfigurableDoorActor::FindDoorComponentByName(const FName& ComponentName) const
{
    TArray<USceneComponent*> Components;
    GetComponents<USceneComponent>(Components);

    for (USceneComponent* Component : Components)
    {
        if (Component && Component->GetFName() == ComponentName)
        {
            return Component;
        }
    }

    return nullptr;
}

void AConfigurableDoorActor::CacheDoorComponents()
{
    LeftDoorComponent = FindDoorComponentByName(LeftDoorComponentName);
    RightDoorComponent = FindDoorComponentByName(RightDoorComponentName);
}

void AConfigurableDoorActor::StartTransition()
{
    if (!LeftDoorComponent && !RightDoorComponent)
    {
        return;
    }

    if (!bUseSmoothTransition || TransitionDuration <= KINDA_SMALL_NUMBER)
    {
        FinishTransition(TransitionTargetAlpha);
        return;
    }

    TransitionStartAlpha = CurrentAlpha;
    TransitionElapsed = 0.f;
    bIsTransitioning = true;
}

void AConfigurableDoorActor::FinishTransition(float FinalAlpha)
{
    bIsTransitioning = false;
    TransitionElapsed = 0.f;
    TransitionStartAlpha = FinalAlpha;
    TransitionTargetAlpha = FinalAlpha;
    ApplyAlpha(FinalAlpha);
}

void AConfigurableDoorActor::ApplyAlpha(float Alpha)
{
    CurrentAlpha = Alpha;

    if (LeftDoorComponent)
    {
        LeftDoorComponent->SetRelativeLocation(
            LerpVector(LeftDoorClosedRelativeLocation, LeftDoorOpenRelativeLocation, Alpha));
    }

    if (RightDoorComponent)
    {
        RightDoorComponent->SetRelativeLocation(
            LerpVector(RightDoorClosedRelativeLocation, RightDoorOpenRelativeLocation, Alpha));
    }
}

FVector AConfigurableDoorActor::LerpVector(const FVector& Start, const FVector& End, float Alpha) const
{
    return FMath::Lerp(Start, End, Alpha);
}
