#include "ControlRoomVolume.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MonsterSpawnManager.h"
#include "MyPaperCharacter.h"

// Sets default values
AControlRoomVolume::AControlRoomVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Box->SetGenerateOverlapEvents(true);

	// 기본 크기 (관제실 담당 팀원이 레벨에서 조정)
	Box->SetBoxExtent(FVector(400.f, 400.f, 300.f));
}

// Called when the game starts or when spawned
void AControlRoomVolume::BeginPlay()
{
	Super::BeginPlay();

	// SpawnManager 찾기 (레벨에 1개 존재한다고 가정)
	SpawnManager = Cast<AMonsterSpawnManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AMonsterSpawnManager::StaticClass())
	);

	if (!SpawnManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ControlRoomVolume] MonsterSpawnManager not found in level."));
	}

	if (Box)
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &AControlRoomVolume::OnBoxBeginOverlap);
		Box->OnComponentEndOverlap.AddDynamic(this, &AControlRoomVolume::OnBoxEndOverlap);
	}
}

void AControlRoomVolume::SetControlRoomFlag(bool bInControlRoom)
{
	if (!SpawnManager) return;

	SpawnManager->SetPlayerInControlRoom(bInControlRoom);

	UE_LOG(LogTemp, Warning, TEXT("[ControlRoomVolume] SetPlayerInControlRoom(%d)"), bInControlRoom ? 1 : 0);
}

void AControlRoomVolume::OnBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player) return;

	SetControlRoomFlag(true);
}

void AControlRoomVolume::OnBoxEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AMyPaperCharacter* Player = Cast<AMyPaperCharacter>(OtherActor);
	if (!Player) return;

	SetControlRoomFlag(false);
}

