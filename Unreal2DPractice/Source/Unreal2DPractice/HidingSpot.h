#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSprite.h"
#include "HidingSpot.generated.h"

class UBoxComponent;
class UPaperSpriteComponent;
class USceneComponent;

UCLASS()
class UNREAL2DPRACTICE_API AHidingSpot : public AActor
{
    GENERATED_BODY()

public:
    AHidingSpot();

protected:
	UFUNCTION()
	void BeginPlay();

	UFUNCTION()
	void OnTriggerBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnTriggerEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* TriggerBox;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UPaperSpriteComponent* Sprite;

    UPROPERTY(EditAnywhere, Category = "Visual")
    UPaperSprite* HideSpriteAsset;

public:
    UBoxComponent* GetTriggerBox() const { return TriggerBox; }

#if WITH_EDITOR
    virtual void OnConstruction(const FTransform& Transform) override;
#endif
};
