#include "SubwayStateActor.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "EndingDirector.h"
#include "InteractWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LevelChangeActor.h"
#include "MyGameInstance.h"
#include "MyPaperCharacter.h"
#include "SubLevelTaskManager.h"
#include "AudioManager.h"
#include "GameSFXData.h"
#include "EngineUtils.h"

ASubwayStateActor::ASubwayStateActor()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentState = ESubwayState::Approaching;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASubwayStateActor::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ASubwayStateActor::OnOverlapEnd);
}

void ASubwayStateActor::BeginPlay()
{
    Super::BeginPlay();

    AutoAssignManagedLevelChangeActors();

    if (!EndingDirector)
    {
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(
            this,
            AEndingDirector::StaticClass(),
            Found
        );

        if (Found.Num() > 0)
        {
            EndingDirector = Cast<AEndingDirector>(Found[0]);
        }
    }

    SetState(CurrentState);

    if (USubLevelTaskManager* TaskManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<USubLevelTaskManager>() : nullptr)
    {
        TaskManager->RefreshSubwayLockStates();
    }
    else
    {
        UpdateManagedLevelChangeActors();
    }
}

void ASubwayStateActor::AutoAssignManagedLevelChangeActors()
{
    if (ManagedLevelChangeActors.Num() > 0 || !GetWorld())
    {
        return;
    }

    const float MaxDistanceSq = FMath::Square(AutoAssignLevelChangeRadius);

    for (TActorIterator<ALevelChangeActor> It(GetWorld()); It; ++It)
    {
        ALevelChangeActor* Candidate = *It;
        if (!Candidate)
        {
            continue;
        }

        const float DistanceSq = FVector::DistSquared(GetActorLocation(), Candidate->GetActorLocation());
        if (DistanceSq <= MaxDistanceSq)
        {
            ManagedLevelChangeActors.Add(Candidate);
        }
    }

    if (ManagedLevelChangeActors.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SubwayStateActor] Auto-linked %d LevelChangeActor(s) for '%s'"),
            ManagedLevelChangeActors.Num(),
            *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[SubwayStateActor] No LevelChangeActor linked to '%s'. Assign ManagedLevelChangeActors or move one within %.0f units."),
            *GetName(),
            AutoAssignLevelChangeRadius);
    }
}

void ASubwayStateActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ActiveWidget)
    {
        UpdateWidgetPosition();
    }
}

void ASubwayStateActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
    if (!Player)
    {
        return;
    }

    Player->SetCurrentSubway(this);
    ShowInteractWidget(Player);
    UE_LOG(LogTemp, Log, TEXT("Player entered subway trigger"));
}

void ASubwayStateActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
    if (!Player)
    {
        return;
    }

    Player->SetCurrentSubway(nullptr);
    HideInteractWidget();
    UE_LOG(LogTemp, Log, TEXT("Player left subway trigger"));
}

void ASubwayStateActor::SetState(ESubwayState NewState)
{
    CurrentState = NewState;

    if (CurrentState == ESubwayState::DoorsOpen)
    {
        AAudioManager* AudioManager =
            Cast<AAudioManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioManager::StaticClass()));

        UMyGameInstance* GI = GetWorld()->GetGameInstance<UMyGameInstance>();

        if (AudioManager && GI && GI->SFXData)
        {
            if (GI->SFXData->SubwayBrake)
            {
                AudioManager->PlaySFX(GI->SFXData->SubwayBrake, GetActorLocation());
            }

            if (GI->SFXData->SubwayBell)
            {
                AudioManager->PlaySFX(GI->SFXData->SubwayBell, GetActorLocation());
            }
        }
    }

    const bool bEnable =
        (CurrentState == ESubwayState::DoorsOpen ||
            CurrentState == ESubwayState::Passed);

    if (TriggerBox)
    {
        TriggerBox->SetCollisionEnabled(
            bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision
        );
    }

    if (!bEnable)
    {
        HideInteractWidget();
    }

    UpdateManagedLevelChangeActors();
}

void ASubwayStateActor::SetReservationActive(bool bActive)
{
    bHasReservation = bActive;
    UpdateManagedLevelChangeActors();
}

void ASubwayStateActor::SetLevelChangeLockActive(bool bLocked)
{
    bLevelChangeLocked = bLocked;
    UpdateManagedLevelChangeActors();
}

void ASubwayStateActor::Interact(AMyPaperCharacter* Player)
{
    if (!Player || !EndingDirector)
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[Interact] CurrentState %s"), StateToString(CurrentState));
    if (CurrentState == ESubwayState::DoorsOpen)
    {
        EndingDirector->StartNormalEnding(
            Player,
            NormalTeleportLocation
        );
    }
    else if (CurrentState == ESubwayState::Passed)
    {
        EndingDirector->StartHiddenEnding(
            Player,
            HiddenTeleportLocation
        );
    }
}

void ASubwayStateActor::ShowInteractWidget(AMyPaperCharacter* Player)
{
    if (!Player || ActiveWidget)
    {
        return;
    }

    CachedPlayer = Player;
    PC = Cast<APlayerController>(Player->GetController());
    if (!PC)
    {
        return;
    }

    UMyGameInstance* GI = GetWorld()->GetGameInstance<UMyGameInstance>();
    if (!GI || !GI->DefaultInteractWidget)
    {
        return;
    }

    ActiveWidget = CreateWidget<UUserWidget>(PC, GI->DefaultInteractWidget);
    if (!ActiveWidget)
    {
        return;
    }

    ActiveWidget->AddToViewport();
    UpdateWidgetPosition();

    if (UInteractWidget* InteractWidget = Cast<UInteractWidget>(ActiveWidget))
    {
        InteractWidget->SetInteractInfo(InteractKey, InteractText);
    }
}

void ASubwayStateActor::HideInteractWidget()
{
    if (ActiveWidget)
    {
        ActiveWidget->RemoveFromParent();
        ActiveWidget = nullptr;
    }

    CachedPlayer = nullptr;
    PC = nullptr;
}

void ASubwayStateActor::UpdateWidgetPosition()
{
    if (!CachedPlayer || !PC || !ActiveWidget)
    {
        return;
    }

    FVector WorldPos = CachedPlayer->GetActorLocation();
    WorldPos += FVector(-100.f, 0.f, 70.f);

    FVector2D ScreenPos;
    PC->ProjectWorldLocationToScreen(WorldPos, ScreenPos);

    ActiveWidget->SetPositionInViewport(ScreenPos, true);
}

void ASubwayStateActor::UpdateManagedLevelChangeActors()
{
    const bool bShouldEnableLevelChange = !bHasReservation && !bLevelChangeLocked;

    for (ALevelChangeActor* LevelChangeActor : ManagedLevelChangeActors)
    {
        if (!LevelChangeActor)
        {
            continue;
        }

        LevelChangeActor->SetLevelChangeEnabled(bShouldEnableLevelChange);
    }
}

const TCHAR* ASubwayStateActor::StateToString(ESubwayState State)
{
    switch (State)
    {
    case ESubwayState::Approaching: return TEXT("Approaching");
    case ESubwayState::DoorsOpen:   return TEXT("DoorsOpen");
    case ESubwayState::Leaving:     return TEXT("Leaving");
    case ESubwayState::Passed:      return TEXT("Passed");
    default:                        return TEXT("Unknown");
    }
}
