#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndingDirector.generated.h"

class AMyPaperCharacter;
class ANormalEndingSequence;
class AHiddenEndingSequence;

UCLASS()
class UNREAL2DPRACTICE_API AEndingDirector : public AActor
{
    GENERATED_BODY()

    public:
    void StartNormalEnding(AMyPaperCharacter* Player, FVector TeleportLocation);
    void StartHiddenEnding(AMyPaperCharacter* Player, FVector TeleportLocation);

private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<ANormalEndingSequence> NormalClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AHiddenEndingSequence> HiddenClass;
};
