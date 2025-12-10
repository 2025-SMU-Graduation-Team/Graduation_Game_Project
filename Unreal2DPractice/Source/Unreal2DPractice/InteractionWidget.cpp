// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "SubLevelTaskManager.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"

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

	if (Circle_A) Circle_A->SetColorAndOpacity(FLinearColor::Gray);
	if (Circle_B) Circle_B->SetColorAndOpacity(FLinearColor::Gray);
	if (Circle_C) Circle_C->SetColorAndOpacity(FLinearColor::Gray);
	if (Circle_D) Circle_D->SetColorAndOpacity(FLinearColor::Gray);
}

void UInteractionWidget::HighlightCircle(UImage* TargetCircle)
{
	if (!TargetCircle) return;

	TargetCircle->SetColorAndOpacity(FLinearColor::Blue);
}

void UInteractionWidget::DisableAllButtons()
{
	if (Station_A)
	{
		Station_A->SetIsEnabled(false);
	}
	if (Station_B)
	{
		Station_B->SetIsEnabled(false);
	}
	if (Station_C)
	{
		Station_C->SetIsEnabled(false);
	}
	if (Station_D)
	{
		Station_D->SetIsEnabled(false);
	}
}

void UInteractionWidget::OnStation_AClicked()
{
	if (bHasSelected) return;
	UE_LOG(LogTemp, Log, TEXT("Station A clicked"));
	bHasSelected = true;

	HighlightCircle(Circle_A);
	DisableAllButtons();

	if (!StationAData) return;
	USubLevelTaskManager::Get(GetWorld())->RequestTask(StationAData);
}

void UInteractionWidget::OnStation_BClicked()
{
	if (bHasSelected) return;
	UE_LOG(LogTemp, Log, TEXT("Station B clicked"));
	bHasSelected = true;

	HighlightCircle(Circle_B);
	DisableAllButtons();
	
	if (!StationBData) return;
	USubLevelTaskManager::Get(GetWorld())->RequestTask(StationBData);
}

void UInteractionWidget::OnStation_CClicked()
{
	if (!bHasSelected) return;
	UE_LOG(LogTemp, Log, TEXT("Station C clicked"));
	bHasSelected = true;

	HighlightCircle(Circle_C);
	DisableAllButtons();

	if (!StationCData) return;
	USubLevelTaskManager::Get(GetWorld())->RequestTask(StationCData);
}

void UInteractionWidget::OnStation_DClicked()
{
	if (!bHasSelected) return;
	UE_LOG(LogTemp, Log, TEXT("Station D clicked"));
	bHasSelected = true;

	HighlightCircle(Circle_D);
	DisableAllButtons();

	if (!StationDData) return;
	USubLevelTaskManager::Get(GetWorld())->RequestTask(StationDData);
}
