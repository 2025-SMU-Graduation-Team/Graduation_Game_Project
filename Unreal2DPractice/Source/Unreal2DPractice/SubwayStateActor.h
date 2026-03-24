#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SubwayStateActor.generated.h"

class UBoxComponent;
class UUserWidget;
class AEndingDirector;
class ALevelChangeActor;
class AMyPaperCharacter;
class APlayerController;
class AActor;

UENUM()
enum class ESubwayState : uint8
{
    Approaching,
    DoorsOpen,
    Leaving,
    Passed
};

UCLASS()
class UNREAL2DPRACTICE_API ASubwayStateActor : public AActor
{
    GENERATED_BODY()

public:
    ASubwayStateActor();

    void Interact(AMyPaperCharacter* Player);
    void SetState(ESubwayState NewState);
    void SetLevelChangeLockActive(bool bLocked);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);

private:
    const TCHAR* StateToString(ESubwayState State);
    void ShowInteractWidget(AMyPaperCharacter* Player);
    void HideInteractWidget();
    void UpdateWidgetPosition();
    void UpdateManagedLevelChangeActors();

private:
    UPROPERTY(EditInstanceOnly)
    AEndingDirector* EndingDirector;

    UPROPERTY(EditAnywhere, Category = "Subway")
    FVector NormalTeleportLocation;

    UPROPERTY(EditAnywhere, Category = "Subway")
    FVector HiddenTeleportLocation;

    UPROPERTY(EditAnywhere, Category = "Subway")
    FName HiddenEndingFinalTargetLevelName;

    UPROPERTY(EditAnywhere, Category = "Subway")
    FText InteractKey = FText::FromString("W");

    UPROPERTY(EditAnywhere, Category = "Subway")
    FText InteractText = FText::FromString("Enter");

    UPROPERTY(EditInstanceOnly, Category = "Subway")
    TArray<TObjectPtr<ALevelChangeActor>> ManagedLevelChangeActors;

    UPROPERTY(EditInstanceOnly, Category = "Subway")
    TArray<TObjectPtr<AActor>> ManagedDoorActors;

    UPROPERTY()
    UUserWidget* ActiveWidget = nullptr;

    UPROPERTY()
    AMyPaperCharacter* CachedPlayer = nullptr;

    UPROPERTY()
    APlayerController* PC = nullptr;

    bool bLevelChangeLocked = false;
    ESubwayState CurrentState;

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerBox;
};
