// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DelayedTaskData.h"
#include "Components/Image.h"
#include "TaskWidgetInterface.h"  
#include "InteractionWidget.generated.h"

class UButton;
class UTexture2D;

UCLASS()
class UNREAL2DPRACTICE_API UInteractionWidget : public UUserWidget, public ITaskWidgetInterface
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void UpdateTaskState_Implementation(bool bTaskRunning) override;

	UPROPERTY(meta=(BindWidget))
	UButton* Station_A;

	UPROPERTY(meta=(BindWidget))
	UButton* Station_B;

	UPROPERTY(meta = (BindWidget))
	UButton* Station_C;

	UPROPERTY(meta = (BindWidget))
	UButton* Station_D;

	UPROPERTY(meta = (BindWidget))
	UImage* Circle_A;
	UPROPERTY(meta = (BindWidget))
	UImage* Circle_B;
	UPROPERTY(meta = (BindWidget))
	UImage* Circle_C;
	UPROPERTY(meta = (BindWidget))
	UImage* Circle_D;

	UFUNCTION()
	void OnStation_AClicked();

	UFUNCTION()
	void OnStation_BClicked();

	UFUNCTION()
	void OnStation_CClicked();

	UFUNCTION()
	void OnStation_DClicked();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	UDelayedTaskData* StationAData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	UDelayedTaskData* StationBData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	UDelayedTaskData* StationCData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	UDelayedTaskData* StationDData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<UTexture2D> DefaultCircleTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<UTexture2D> SelectedCircleTexture;

protected:
	bool bHasSelected = false;

	void HighlightCircle(UImage* TargetCircle);
	void DisableAllButtons();
	void PlaySubwaySelectSound();
	void SetCircleVisual(UImage* TargetCircle, bool bSelected);

	void LockButtons(bool bLock);
};
