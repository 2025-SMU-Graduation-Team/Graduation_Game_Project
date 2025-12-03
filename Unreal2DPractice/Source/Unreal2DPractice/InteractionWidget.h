// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DelayedTaskData.h"
#include "InteractionWidget.generated.h"

class UButton;

UCLASS()
class UNREAL2DPRACTICE_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	UButton* Station_A;

	UPROPERTY(meta=(BindWidget))
	UButton* Station_B;

	UPROPERTY(meta = (BindWidget))
	UButton* Station_C;

	UPROPERTY(meta = (BindWidget))
	UButton* Station_D;

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
};
