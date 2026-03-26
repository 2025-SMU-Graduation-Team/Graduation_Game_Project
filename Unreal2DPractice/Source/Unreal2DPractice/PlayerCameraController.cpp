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
    Camera = GetOwner()->FindComponentByClass<UCameraComponent>();

    if (!SpringArm)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerCameraController: SpringArm not found"));
        return;
    }

    SpringArm->bEnableCameraLag = false;
    SpringArm->bEnableCameraRotationLag = false;
    SpringArm->bDoCollisionTest = false;

    RefreshCameraPosition();
}

void UPlayerCameraController::SetLimitVolume(ACameraLimitVolume* NewVolume)
{
    LimitVolume = NewVolume;

    RefreshCameraPosition();
}

void UPlayerCameraController::ClearLimitVolume(ACameraLimitVolume* VolumeToClear, bool bForceClear)
{
    if (LimitVolume != VolumeToClear)
    {
        return;
    }

    if (bKeepLastLimitWhenExited && !bForceClear)
    {
        return;
    }

    LimitVolume = nullptr;
}

void UPlayerCameraController::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    RefreshCameraPosition();
}

void UPlayerCameraController::RefreshCameraPosition()
{
    if (!SpringArm || !LimitVolume)
    {
        return;
    }

    const FVector ClampedTarget = GetClampedCameraTarget();
    SpringArm->SetWorldLocation(ClampedTarget);
    SpringArm->UpdateComponentToWorld();

    if (Camera)
    {
        Camera->UpdateComponentToWorld();
    }
}

float UPlayerCameraController::GetHalfViewHeight() const
{
    if (!Camera || !SpringArm)
    {
        return 0.f;
    }

    const float FovRad = FMath::DegreesToRadians(Camera->FieldOfView * 0.5f);
    const float Distance = SpringArm->TargetArmLength;

    return FMath::Tan(FovRad) * Distance;
}


FVector UPlayerCameraController::GetClampedCameraTarget() const
{
    const FVector PlayerLoc = GetOwner()->GetActorLocation();
    const FBox Bounds = LimitVolume->GetLimitBounds();

    FVector Result = PlayerLoc + cameraOffset;

    const float HalfViewWidth = SpringArm->TargetArmLength;
    const float HalfViewHeight = GetHalfViewHeight();

    Result.X = FMath::Clamp(PlayerLoc.X, Bounds.Min.X + HalfViewWidth, Bounds.Max.X - HalfViewWidth);
    const float MaxCameraZ = Bounds.Max.Z - HalfViewHeight;
    if (Result.Z > MaxCameraZ)
    {
        Result.Z = MaxCameraZ;
    }

    return Result;
}
