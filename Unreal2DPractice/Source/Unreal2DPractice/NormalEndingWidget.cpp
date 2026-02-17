#include "NormalEndingWidget.h"

void UNormalEndingWidget::StartShake(float Duration)
{
    ShakeDuration = Duration;
    ShakeElapsed = 0.f;
    bShaking = true;

    OriginalTranslation = RenderTransform.Translation;

    NextImpactTime = FMath::FRandRange(0.6f, 1.3f);
    ImpactStrength = 0.f;
    ResidualTime = 0.f;
}

void UNormalEndingWidget::StopShake()
{
    bShaking = false;
    SetRenderTranslation(OriginalTranslation);
}

void UNormalEndingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!bShaking)
        return;

    ShakeElapsed += InDeltaTime;

    FVector2D Offset = FVector2D::ZeroVector;

    if (ShakeElapsed >= NextImpactTime)
    {
        ImpactStrength = FMath::FRandRange(15.f, 25.f);
        ResidualTime = 0.35f;

        NextImpactTime = ShakeElapsed + FMath::FRandRange(1.0f, 1.8f);
    }

    if (ResidualTime > 0.f)
    {
        ResidualTime -= InDeltaTime;

        Offset.X += FMath::FRandRange(-2.f, 2.f);
        Offset.Y += FMath::FRandRange(-1.5f, 1.5f);
    }

    if (ImpactStrength > 0.f)
    {
        Offset.Y += FMath::FRandRange(-ImpactStrength, ImpactStrength);

        ImpactStrength *= 0.82f;
        if (ImpactStrength < 1.f)
            ImpactStrength = 0.f;
    }

    SetRenderTranslation(OriginalTranslation + Offset);

    if (ShakeElapsed >= ShakeDuration)
    {
        StopShake();
    }
}
