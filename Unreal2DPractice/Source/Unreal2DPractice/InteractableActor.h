// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

class UBoxComponent;
class AMyPaperCharacter;

UCLASS(Abstract)
class UNREAL2DPRACTICE_API AInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();
	virtual void Interact();
	virtual void InteractFromEnterKey();
	bool CanInteractFromEnterKey() const;
	FText GetDisplayInteractKey() const;
	void SetInteractionEnabled(bool bEnabled);
	bool IsInteractionEnabled() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Tick(float DeltaTime);

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY()
	UUserWidget* ActiveWidget;

	UPROPERTY(EditAnywhere, Category = "Interact")
	FText InteractKey = FText::FromString("E");

	UPROPERTY(EditAnywhere, Category = "Interact")
	FText InteractText = FText::FromString("Interact");

	UPROPERTY(EditAnywhere, Category = "Interact")
	bool bAllowEnterKeyInteraction = false;

	UPROPERTY(EditAnywhere, Category = "Interact")
	FText AlternateInteractKey;

	UPROPERTY()
	AMyPaperCharacter* CachedPlayer;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* CurrentPanel = nullptr;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateWidgetPosition();

	APlayerController* PC;

private:
	bool bInteractionEnabled = true;
};
