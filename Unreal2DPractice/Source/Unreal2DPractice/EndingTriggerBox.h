#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndingTriggerBox.generated.h"

class UBoxComponent;
class UUserWidget;

UCLASS()
class UNREAL2DPRACTICE_API AEndingTriggerBox : public AActor
{
	GENERATED_BODY()

public:
	AEndingTriggerBox();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(VisibleAnywhere, Category = "Ending")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Ending")
	TSubclassOf<UUserWidget> EndingWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Ending")
	int32 WidgetZOrder = 100;

	UPROPERTY(EditAnywhere, Category = "Ending")
	bool bPauseGameOnTrigger = true;

	UPROPERTY(EditAnywhere, Category = "Ending")
	bool bShowMouseCursorOnTrigger = true;

	UPROPERTY(EditAnywhere, Category = "Ending")
	bool bTriggerOnce = true;

private:
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> ActiveWidget;

	bool bHasTriggered = false;
};
