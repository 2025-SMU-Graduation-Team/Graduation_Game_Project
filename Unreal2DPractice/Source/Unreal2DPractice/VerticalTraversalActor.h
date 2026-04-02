#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "VerticalTraversalActor.generated.h"

class UPaperFlipbook;
class AMyPaperCharacter;

UCLASS()
class UNREAL2DPRACTICE_API AVerticalTraversalActor : public AInteractableActor
{
	GENERATED_BODY()

public:
	AVerticalTraversalActor();
	void SetTraversalEnabled(bool bEnabled);

protected:
	virtual void BeginPlay() override;
	virtual void Interact() override;
	virtual void Tick(float DeltaTime) override;

private:
	void FinishTraversal();
	void UpdateTraversalPosition(float DeltaTime);

private:
	UPROPERTY(EditAnywhere, Category = "Traversal")
	TObjectPtr<UPaperFlipbook> ClimbAnimation;

	UPROPERTY(EditAnywhere, Category = "Traversal", meta = (ClampMin = "0.0"))
	float TraversalDuration = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Traversal")
	FName TargetLevelName;

	UPROPERTY(EditAnywhere, Category = "Traversal")
	FVector TargetTeleportLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Traversal")
	bool bRestoreMovementAfterTraversal = true;

	UPROPERTY(EditAnywhere, Category = "Traversal", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float VisualClimbAlpha = 0.33f;

	UPROPERTY()
	TObjectPtr<AMyPaperCharacter> TraversingPlayer;

	FTimerHandle TraversalTimerHandle;
	FVector TraversalStartLocation = FVector::ZeroVector;
	float TraversalElapsedTime = 0.0f;
	bool bIsTraversing = false;
};
