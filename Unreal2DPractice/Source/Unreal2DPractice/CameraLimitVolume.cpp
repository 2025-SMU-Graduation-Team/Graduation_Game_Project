#include "CameraLimitVolume.h"
#include "Components/BoxComponent.h"
#include "PlayerCameraController.h"

ACameraLimitVolume::ACameraLimitVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    RootComponent = Box;

    Box->SetCollisionProfileName(TEXT("OverlapAll"));
}

void ACameraLimitVolume::BeginPlay()
{
    Super::BeginPlay();

    Box->OnComponentBeginOverlap.AddDynamic(
        this,
        &ACameraLimitVolume::OnOverlapBegin
    );

    Box->OnComponentEndOverlap.AddDynamic(
        this,
        &ACameraLimitVolume::OnOverlapEnd
    );
}

void ACameraLimitVolume::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!OtherActor) return;

    UPlayerCameraController* CameraController = OtherActor->FindComponentByClass<UPlayerCameraController>();

    if (CameraController)
    {
        CameraController->SetLimitVolume(this);
    }
}

void ACameraLimitVolume::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    if (!OtherActor) return;

    UPlayerCameraController* CameraController =
        OtherActor->FindComponentByClass<UPlayerCameraController>();

    if (CameraController)
    {
        CameraController->ClearLimitVolume(this);
    }
}

FBox ACameraLimitVolume::GetLimitBounds() const
{
    return Box->Bounds.GetBox();
}

bool ACameraLimitVolume::ContainsWorldLocation(const FVector& WorldLocation) const
{
    if (!Box)
    {
        return false;
    }

    const FVector LocalPoint = Box->GetComponentTransform().InverseTransformPosition(WorldLocation);
    const FVector Extent = Box->GetScaledBoxExtent();

    return
        FMath::Abs(LocalPoint.X) <= Extent.X &&
        FMath::Abs(LocalPoint.Y) <= Extent.Y &&
        FMath::Abs(LocalPoint.Z) <= Extent.Z;
}
