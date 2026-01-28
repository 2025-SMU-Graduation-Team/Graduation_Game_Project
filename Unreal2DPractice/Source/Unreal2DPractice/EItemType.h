// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Normal      UMETA(DisplayName="Normal"),
    Wallet      UMETA(DisplayName="Wallet"),
    Ghost       UMETA(DisplayName="Ghost"),
    Nipper       UMETA(DisplayName = "Nipper"),
    Screwdirver       UMETA(DisplayName = "Screwdirver")
};