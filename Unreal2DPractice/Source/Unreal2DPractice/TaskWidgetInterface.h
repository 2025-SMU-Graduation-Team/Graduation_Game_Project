#pragma once

#include "UObject/Interface.h"
#include "TaskWidgetInterface.generated.h"

UINTERFACE(Blueprintable)
class UTaskWidgetInterface : public UInterface
{
    GENERATED_BODY()
    };

class ITaskWidgetInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void UpdateTaskState(bool bRunning);
};
