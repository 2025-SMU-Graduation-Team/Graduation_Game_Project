// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "SubLevelTaskManager.h"
#include "EngineUtils.h"

void UInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Station_A)
	{
		Station_A->OnClicked.AddDynamic(this, &UInteractionWidget::OnStation_AClicked);
	}

	if (Station_B)
	{
		Station_B->OnClicked.AddDynamic(this, &UInteractionWidget::OnStation_BClicked);
	}

	if (Station_C)
	{
		Station_C->OnClicked.AddDynamic(this, &UInteractionWidget::OnStation_CClicked);
	}

	if (Station_D)
	{
		Station_D->OnClicked.AddDynamic(this, &UInteractionWidget::OnStation_DClicked);
	}
}

void UInteractionWidget::OnStation_AClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Station A clicked"));
	USubLevelTaskManager::Get(GetWorld())->RequestTask(TEXT("StationA"));
}

void UInteractionWidget::OnStation_BClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Station B clicked"));
	USubLevelTaskManager::Get(GetWorld())->RequestTask(TEXT("StationB"));
}

void UInteractionWidget::OnStation_CClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Station C clicked"));
	USubLevelTaskManager::Get(GetWorld())->RequestTask(TEXT("StationC"));
}

void UInteractionWidget::OnStation_DClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Station D clicked"));
	USubLevelTaskManager::Get(GetWorld())->RequestTask(TEXT("StationD"));
}
