#include "EarthquakeEventActor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

AEarthquakeEventActor::AEarthquakeEventActor()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AEarthquakeEventActor::BeginPlay()
{
    Super::BeginPlay();

    if (!TriggerBox)
    {
        UE_LOG(LogTemp, Error, TEXT("TriggerBox is null"));
        return;
    }

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AEarthquakeEventActor::OnTriggerBeginOverlap);
}

void AEarthquakeEventActor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bEventStarted)
        return;

    if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
    {
        bEventStarted = true;

        GetWorldTimerManager().SetTimer(PreviewShakeTimer, this, &AEarthquakeEventActor::StartPreviewShake,
            FirstShakeDelay, false);
    }
}

void AEarthquakeEventActor::StartPreviewShake()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->ClientStartCameraShake(CameraShake, ShakeScale);
    }

    GetWorldTimerManager().SetTimer(
        StopShakeTimer,
        this,
        &AEarthquakeEventActor::StopShake,
        PreviewShakeDuration,
        false
    );

    GetWorldTimerManager().SetTimer(
        CollapseStartTimer,
        this,
        &AEarthquakeEventActor::StartCollapse,
        CollapseDelayAfterPreview,
        false
    );
}

void AEarthquakeEventActor::StartCollapse()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->ClientStartCameraShake(CameraShake, ShakeScale);
    }

    SpawnedRubbleCount = 0;

    GetWorldTimerManager().SetTimer(
        RubbleSpawnTimer,
        this,
        &AEarthquakeEventActor::SpawnRubble,
        RubbleSpawnInterval,
        true
    );
}

void AEarthquakeEventActor::SpawnRubble()
{
    if (BigRubbles.Num() == 0 && SmallRubbles.Num() == 0)
    {
        StopShake();
        return;
    }

    if (SpawnedRubbleCount >= RubbleSpawnCount)
    {
        GetWorldTimerManager().ClearTimer(RubbleSpawnTimer);
        StopShake();
        return;
    }

    float Rand = FMath::FRand();
    bool bSpawnBig = Rand < BigSpawnRatio;

    TSubclassOf<AActor> SelectedClass = nullptr;

    if (bSpawnBig && BigRubbles.Num() > 0)
    {
        int32 Index = FMath::RandRange(0, BigRubbles.Num() - 1);
        SelectedClass = BigRubbles[Index];
    }
    else if (SmallRubbles.Num() > 0)
    {
        int32 Index = FMath::RandRange(0, SmallRubbles.Num() - 1);
        SelectedClass = SmallRubbles[Index];
        bSpawnBig = false;
    }

    if (!SelectedClass)
        return;

    FVector SpawnLoc = RubbleSpawnPoint;
    SpawnLoc.X += FMath::RandRange(-120.f, 120.f);
    SpawnLoc.Y += FMath::RandRange(-60.f, 60.f);

    float RandomYaw = FMath::RandRange(-45.f, 45.f);
    FRotator SpawnRot(0.f, 0.f, RandomYaw);

    AActor* Spawned = GetWorld()->SpawnActor<AActor>(
        SelectedClass,
        SpawnLoc,
        SpawnRot
    );

    if (Spawned)
    {
        UPrimitiveComponent* Prim =
            Cast<UPrimitiveComponent>(Spawned->GetRootComponent());

        if (Prim && Prim->IsSimulatingPhysics())
        {
            float DownImpulse = bSpawnBig ? BigDownImpulse : SmallDownImpulse;
            Prim->AddImpulse(FVector(0.f, 0.f, DownImpulse), NAME_None, true);

            float TorqueStrength = bSpawnBig ? 600.f : 300.f;
            float Torque = FMath::RandRange(-TorqueStrength, TorqueStrength);
            Prim->AddTorqueInRadians(FVector(0.f, 0.f, Torque));

            if (bSpawnBig)
            {
                Prim->SetMassOverrideInKg(NAME_None, 300.f);
            }
            else
            {
                Prim->SetMassOverrideInKg(NAME_None, 120.f);
            }
        }
    }
    SpawnedRubbleCount++;
}

void AEarthquakeEventActor::StopShake()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->ClientStopCameraShake(CameraShake, false);
    }
}
