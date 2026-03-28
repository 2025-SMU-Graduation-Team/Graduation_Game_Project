#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionManager.generated.h"

class AMyPaperCharacter;
class ULevel;

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
	void FinishTransition();
	void OnLevelLoaded(ULevel* LoadedLevel, UWorld* World);
	void LoadLevel(FName LevelName);
	void UnloadLevel(FName LevelName);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LevelTransition", meta = (AllowPrivateAccess = "true"))
	FName CurrentSubLevel;

	UPROPERTY()
	bool bIsTransitioning;

	UPROPERTY()
	FName PendingNextLevel;

	UPROPERTY()
	FName PendingPreviousLevel;

	UPROPERTY()
	TObjectPtr<AMyPaperCharacter> PendingTeleportPlayer;

	UPROPERTY()
	FVector PendingTeleportLocation = FVector::ZeroVector;
};
