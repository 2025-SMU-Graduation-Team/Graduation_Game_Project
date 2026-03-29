#include "MonsterSpawnAreaVolume.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "MonsterSpawnManager.h"
#include "MyPaperCharacter.h"

AMonsterSpawnAreaVolume::AMonsterSpawnAreaVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Box->SetGenerateOverlapEvents(true);

	Box->SetBoxExtent(FVector(1000.f, 300.f, 300.f));
}

void AMonsterSpawnAreaVolume::BeginPlay()
{
	Super::BeginPlay();

	SpawnManager = Cast<AMonsterSpawnManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AMonsterSpawnManager::StaticClass())
	);

	if (!SpawnManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MonsterSpawnAreaVolume] MonsterSpawnManager not found."));
	}

	if (Box)
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &AMonsterSpawnAreaVolume::OnBoxBeginOverlap);
		Box->OnComponentEndOverlap.AddDynamic(this, &AMonsterSpawnAreaVolume::OnBoxEndOverlap);
	}

	GetWorldTimerManager().SetTimerForNextTick(this, &AMonsterSpawnAreaVolume::CheckInitialPlayerOverlap);
}

void AMonsterSpawnAreaVolume::CheckInitialPlayerOverlap()
{
	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player || !SpawnManager || !Box)
	{
		return;
	}

	if (Box->IsOverlappingActor(Player))
	{
		SpawnManager->SetPlayerInSpawnArea(true);
		UE_LOG(LogTemp, Warning, TEXT("[MonsterSpawnAreaVolume] Player already inside spawn area at startup"));
	}
}

void AMonsterSpawnAreaVolume::OnBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player || !SpawnManager)
	{
		return;
	}

	SpawnManager->SetPlayerInSpawnArea(true);

	UE_LOG(LogTemp, Warning, TEXT("[MonsterSpawnAreaVolume] Player entered spawn area"));
}

void AMonsterSpawnAreaVolume::OnBoxEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player || !SpawnManager)
	{
		return;
	}

	SpawnManager->SetPlayerInSpawnArea(false);

	UE_LOG(LogTemp, Warning, TEXT("[MonsterSpawnAreaVolume] Player left spawn area"));
}
