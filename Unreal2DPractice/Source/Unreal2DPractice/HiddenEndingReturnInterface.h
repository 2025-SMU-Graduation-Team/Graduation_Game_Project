#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HiddenEndingReturnInterface.generated.h"

UINTERFACE(Blueprintable)
class UHiddenEndingReturnInterface : public UInterface
{
    GENERATED_BODY()
};

class IHiddenEndingReturnInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HiddenEnding")
    void HandleHiddenEndingReturn();
};
