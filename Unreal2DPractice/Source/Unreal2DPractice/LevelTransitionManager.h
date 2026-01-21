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

	void ChangeSubLevel(FName LevelName);

protected:
	virtual void BeginPlay() override;

private:
	void LoadLevel(FName LevelName);
	void UnloadLevel(FName LevelName);

private:
	UPROPERTY()
	FName CurrentSubLevel;

	UPROPERTY()
	bool bIsTransitioning;
};
