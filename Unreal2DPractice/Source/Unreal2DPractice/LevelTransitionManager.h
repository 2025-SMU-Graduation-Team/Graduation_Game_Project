#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionManager.generated.h"

class AMyPaperCharacter;

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
	void FinishTransition(FName NextLevel, AMyPaperCharacter* PlayerToTeleport, const FVector& TeleportLocation);
	void LoadLevel(FName LevelName);
	void UnloadLevel(FName LevelName);
	bool IsPersistentLevelTarget(FName LevelName) const;
	FName GetPersistentLevelName() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LevelTransition", meta = (AllowPrivateAccess = "true"))
	FName CurrentSubLevel;

	UPROPERTY()
	bool bIsTransitioning;
};
