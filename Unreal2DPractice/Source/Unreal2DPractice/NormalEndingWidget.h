#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NormalEndingWidget.generated.h"

UCLASS()
class UNREAL2DPRACTICE_API UNormalEndingWidget : public UUserWidget
{
    GENERATED_BODY()

    public:
    void StartShake(float Duration);
    void StopShake();

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    float ShakeDuration = 0.f;
    float ShakeElapsed = 0.f;

    float NextImpactTime = 0.f;
    float ImpactStrength = 0.f;
    float ResidualTime = 0.f;

    bool bShaking = false;

    FVector2D OriginalTranslation = FVector2D::ZeroVector;
};
