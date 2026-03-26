#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionManager.generated.h"

class AMyPaperCharacter;
class ULevel;
class APlayerController;

UCLASS()
class UNREAL2DPRACTICE_API ALevelTransitionManager : public AActor
{
	GENERATED_BODY()

	public:
	ALevelTransitionManager();

	static ALevelTransitionManager* Get(UWorld* World);

	void ChangeSubLevel(FName LevelName, AMyPaperCharacter* PlayerToTeleport = nullptr, const FVector& TeleportLocation = FVector::ZeroVector);

protected:
	virtual void BeginPlay() override;

private:
	void BeginFadeTransition(FName NextLevel, AMyPaperCharacter* PlayerToTeleport, const FVector& TeleportLocation);
	void FinishTransition(FName NextLevel, AMyPaperCharacter* PlayerToTeleport, const FVector& TeleportLocation);
	void CompleteTransition(AMyPaperCharacter* PlayerToTeleport);
	void LoadLevel(FName LevelName);
	void UnloadLevel(FName LevelName);
	bool IsPersistentLevelTarget(FName LevelName) const;
	FName GetPersistentLevelName() const;
	APlayerController* GetPrimaryPlayerController() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LevelTransition", meta = (AllowPrivateAccess = "true"))
	FName CurrentSubLevel;

	UPROPERTY()
	bool bIsTransitioning;

	UPROPERTY(EditAnywhere, Category = "LevelTransition")
	float TransitionFadeDuration = 0.2f;

	UPROPERTY(EditAnywhere, Category = "LevelTransition")
	float FadeInStartDelay = 0.02f;
};
