#include "SubwayStateActor.h"
#include "EndingDirector.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyPaperCharacter.h"

ASubwayStateActor::ASubwayStateActor()
{
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
}

void ASubwayStateActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
    if (Player)
    {
        Player->SetCurrentSubway(this);
        UE_LOG(LogTemp, Log, TEXT("Player entered subway trigger"));
    }
}

void ASubwayStateActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
    if (Player)
    {
        Player->SetCurrentSubway(nullptr);
        UE_LOG(LogTemp, Log, TEXT("Player left subway trigger"));
    }
}


void ASubwayStateActor::SetState(ESubwayState NewState)
{
    CurrentState = NewState;

    const bool bEnable =
        (CurrentState == ESubwayState::DoorsOpen ||
            CurrentState == ESubwayState::Passed);

    if (TriggerBox)
    {
        TriggerBox->SetCollisionEnabled(
            bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision
        );
    }
}

void ASubwayStateActor::Interact(AMyPaperCharacter* Player)
{
    if (!Player || !EndingDirector)
        return;

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
