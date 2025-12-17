// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OpeningDoorInterface.generated.h"

UINTERFACE(Blueprintable)
class UOpeningDoorInterface : public UInterface
{
    GENERATED_BODY()
};

class IOpeningDoorInterface
{
    GENERATED_BODY()

    public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OpenDoor();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void CloseDoor();
};
