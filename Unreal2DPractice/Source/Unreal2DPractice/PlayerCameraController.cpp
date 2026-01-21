#include "PlayerCameraController.h"
#include "GameFramework/SpringArmComponent.h"
#include "CameraLimitVolume.h"
#include "GameFramework/Actor.h"

UPlayerCameraController::UPlayerCameraController()
{
    PrimaryComponentTick.bCanEverTick = true;

    SpringArm = nullptr;
    LimitVolume = nullptr;
}

void UPlayerCameraController::BeginPlay()
{
    Super::BeginPlay();

    SpringArm = GetOwner()->FindComponentByClass<USpringArmComponent>();
    if (!SpringArm)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerCameraController: SpringArm not found"));
        return;
    }

    SpringArm->bEnableCameraLag = false;
    SpringArm->bEnableCameraRotationLag = false;
}

void UPlayerCameraController::SetLimitVolume(ACameraLimitVolume* NewVolume)
{
    LimitVolume = NewVolume;
}

void UPlayerCameraController::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!SpringArm || !LimitVolume) return;

    const FVector ClampedTarget = GetClampedCameraTarget();
    SpringArm->SetWorldLocation(ClampedTarget);
}

FVector UPlayerCameraController::GetClampedCameraTarget() const
{
    const FVector PlayerLoc = GetOwner()->GetActorLocation();
    const FBox Bounds = LimitVolume->GetLimitBounds();

    FVector Result = PlayerLoc;

    const float HalfViewWidth = SpringArm->TargetArmLength;

    Result.X = FMath::Clamp(
        PlayerLoc.X,
        Bounds.Min.X + HalfViewWidth,
        Bounds.Max.X - HalfViewWidth
    );

    return Result;
}
