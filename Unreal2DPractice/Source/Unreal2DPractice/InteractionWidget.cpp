// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "SubLevelTaskManager.h"
#include "UISelectedManager.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "AudioManager.h"
#include "MyGameInstance.h"
#include "GameSFXData.h"

void UInteractionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UUISelectedManager* State = GetGameInstance()->GetSubsystem<UUISelectedManager>();

    if (Station_A)
        Station_A->OnClicked.AddDynamic(this, &UInteractionWidget::OnStation_AClicked);
    if (Station_B)
        Station_B->OnClicked.AddDynamic(this, &UInteractionWidget::OnStation_BClicked);
    if (Station_C)
        Station_C->OnClicked.AddDynamic(this, &UInteractionWidget::OnStation_CClicked);
    if (Station_D)
        Station_D->OnClicked.AddDynamic(this, &UInteractionWidget::OnStation_DClicked);

    if (Circle_A) Circle_A->SetColorAndOpacity(FLinearColor::Gray);
    if (Circle_B) Circle_B->SetColorAndOpacity(FLinearColor::Gray);
    if (Circle_C) Circle_C->SetColorAndOpacity(FLinearColor::Gray);
    if (Circle_D) Circle_D->SetColorAndOpacity(FLinearColor::Gray);

    if (State->LastSelectedStation == "StationA") {
        HighlightCircle(Circle_A);
        bHasSelected = true;
        DisableAllButtons();
    }
    else if (State->LastSelectedStation == "StationB") {
        HighlightCircle(Circle_B);
        bHasSelected = true;
        DisableAllButtons();
    }
    else if (State->LastSelectedStation == "StationC") {
        HighlightCircle(Circle_C);
        bHasSelected = true;
        DisableAllButtons();
    }
    else if (State->LastSelectedStation == "StationD") {
        HighlightCircle(Circle_D);
        bHasSelected = true;
        DisableAllButtons();
    }

    if (USubLevelTaskManager* TaskManager = GetGameInstance()->GetSubsystem<USubLevelTaskManager>())
    {
        TaskManager->RegisterWidget(this);
    }
}

void UInteractionWidget::UpdateTaskState_Implementation(bool bTaskRunning)
{
    LockButtons(bTaskRunning);
}

void UInteractionWidget::HighlightCircle(UImage* TargetCircle)
{
    if (!TargetCircle) return;

    TargetCircle->SetColorAndOpacity(FLinearColor::Yellow);
}

void UInteractionWidget::DisableAllButtons()
{
    if (Station_A) Station_A->SetIsEnabled(false);
    if (Station_B) Station_B->SetIsEnabled(false);
    if (Station_C) Station_C->SetIsEnabled(false);
    if (Station_D) Station_D->SetIsEnabled(false);
}

void UInteractionWidget::PlaySubwaySelectSound()
{
    AAudioManager* AudioManager =
        Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));

    UMyGameInstance* GI =
        Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

    if (AudioManager && GI && GI->SFXData && GI->SFXData->Mini_SubwaySelect)
    {
        AudioManager->PlaySFX2D(GI->SFXData->Mini_SubwaySelect);
    }
}

void UInteractionWidget::LockButtons(bool bLock)
{
    Station_A->SetIsEnabled(!bLock);
    Station_B->SetIsEnabled(!bLock);
    Station_C->SetIsEnabled(!bLock);
    Station_D->SetIsEnabled(!bLock);
}

void UInteractionWidget::OnStation_AClicked()
{
    if (bHasSelected) return;
    UE_LOG(LogTemp, Log, TEXT("Station A clicked"));
    bHasSelected = true;
    PlaySubwaySelectSound();

    HighlightCircle(Circle_A);
    DisableAllButtons();

    UUISelectedManager* State = GetGameInstance()->GetSubsystem<UUISelectedManager>();
    State->LastSelectedStation = "StationA";

    if (!StationAData) return;
    if (USubLevelTaskManager* TaskManager = GetGameInstance()->GetSubsystem<USubLevelTaskManager>())
    {
        TaskManager->RequestTask(StationAData);
    }
}

void UInteractionWidget::OnStation_BClicked()
{
    if (bHasSelected) return;
    UE_LOG(LogTemp, Log, TEXT("Station B clicked"));
    bHasSelected = true;
    PlaySubwaySelectSound();

    HighlightCircle(Circle_B);
    DisableAllButtons();

    UUISelectedManager* State = GetGameInstance()->GetSubsystem<UUISelectedManager>();
    State->LastSelectedStation = "StationB";

    if (!StationBData) return;
    if (USubLevelTaskManager* TaskManager = GetGameInstance()->GetSubsystem<USubLevelTaskManager>())
    {
        TaskManager->RequestTask(StationBData);
    }
}

void UInteractionWidget::OnStation_CClicked()
{
    if (bHasSelected) return;
    UE_LOG(LogTemp, Log, TEXT("Station C clicked"));
    bHasSelected = true;
    PlaySubwaySelectSound();

    HighlightCircle(Circle_C);
    DisableAllButtons();

    UUISelectedManager* State = GetGameInstance()->GetSubsystem<UUISelectedManager>();
    State->LastSelectedStation = "StationC";

    if (!StationCData) return;
    if (USubLevelTaskManager* TaskManager = GetGameInstance()->GetSubsystem<USubLevelTaskManager>())
    {
        TaskManager->RequestTask(StationCData);
    }
}

void UInteractionWidget::OnStation_DClicked()
{
    if (bHasSelected) return;
    UE_LOG(LogTemp, Log, TEXT("Station D clicked"));
    bHasSelected = true;
    PlaySubwaySelectSound();

    HighlightCircle(Circle_D);
    DisableAllButtons();

    UUISelectedManager* State = GetGameInstance()->GetSubsystem<UUISelectedManager>();
    State->LastSelectedStation = "StationD";

    if (!StationDData) return;
    if (USubLevelTaskManager* TaskManager = GetGameInstance()->GetSubsystem<USubLevelTaskManager>())
    {
        TaskManager->RequestTask(StationDData);
    }
}

