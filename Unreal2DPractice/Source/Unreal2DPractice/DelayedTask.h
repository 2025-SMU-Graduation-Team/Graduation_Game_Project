#pragma once

#include "CoreMinimal.h"
#include "DelayedTask.generated.h"

USTRUCT(BlueprintType)
struct FDelayedTask
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* TargetActor = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform TargetTransform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Delay = 5.f;
};
