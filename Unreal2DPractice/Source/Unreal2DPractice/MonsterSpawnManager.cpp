// MonsterSpawnManager.cpp

#include "MonsterSpawnManager.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "EngineUtils.h"                 // TActorIterator
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

#include "MyPaperMonster.h"
#include "MyPaperCharacter.h"
#include "MyGameInstance.h"
#include "AudioManager.h"
#include "GameSFXData.h"

AMonsterSpawnManager::AMonsterSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMonsterSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	// ������ġ: min/max ������ ������ ��ü
	if (MaxSpawnInterval < MinSpawnInterval)
	{
		Swap(MaxSpawnInterval, MinSpawnInterval);
	}
	if (MaxSpawnDistance < MinSpawnDistance)
	{
		Swap(MaxSpawnDistance, MinSpawnDistance);
	}

	ScheduleNextSpawn();
}

void AMonsterSpawnManager::SetPlayerInControlRoom(bool bInControlRoom)
{
	bPlayerInControlRoom = bInControlRoom;

	UE_LOG(LogTemp, Warning, TEXT("[MonsterSpawnManager] ControlRoom=%d"), bPlayerInControlRoom ? 1 : 0);
}

void AMonsterSpawnManager::RequestEventSpawn()
{
	TrySpawn(ESpawnReason::Event);
}

void AMonsterSpawnManager::ForceDespawnMonster()
{
    if (!CurrentMonster.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("[MonsterSpawnManager] ForceDespawnMonster: no monster"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[MonsterSpawnManager] ForceDespawn %s"), *GetNameSafe(CurrentMonster.Get()));
    CurrentMonster->Destroy();
    CurrentMonster = nullptr;
}

void AMonsterSpawnManager::ScheduleNextSpawn()
{
	const float Delay = FMath::FRandRange(MinSpawnInterval, MaxSpawnInterval);

	GetWorldTimerManager().SetTimer(
		TimerHandle_Spawn,
		this,
		&AMonsterSpawnManager::TrySpawnRandom,
		Delay,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("[MonsterSpawnManager] Next random spawn attempt in %.2f sec"), Delay);
}

void AMonsterSpawnManager::TrySpawnRandom()
{
	// ���� �õ��� ������ ���� (���� ����/���п� �����ϰ� ���?����)
	ScheduleNextSpawn();

	TrySpawn(ESpawnReason::Random);
}

bool AMonsterSpawnManager::IsMonsterAlive()
{
	// 1) Weak ptr ����
	if (CurrentMonster.IsValid())
	{
		return true;
	}

	// 2) Ȥ�� ptr�� ����µ�?���忡 �����ִ� ���?���?
	if (!GetWorld()) return false;

	for (TActorIterator<AMyPaperMonster> It(GetWorld()); It; ++It)
	{
		if (IsValid(*It))
		{
			CurrentMonster = *It;
			return true;
		}
	}

	return false;
}

AMyPaperCharacter* AMonsterSpawnManager::GetPlayer() const
{
	return Cast<AMyPaperCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

FVector AMonsterSpawnManager::ComputeSpawnLocation(const AMyPaperCharacter* Player, bool bSpawnFromLeft, float SpawnDistance) const
{
	FVector Loc = Player->GetActorLocation();
	Loc.X += (bSpawnFromLeft ? -1.f : 1.f) * SpawnDistance;
	Loc.Y += SpawnYOffset;
	Loc.Z += SpawnZOffset;

	return Loc;
}

void AMonsterSpawnManager::TrySpawn(ESpawnReason Reason)
{
	if (bPlayerInControlRoom)
	{
		UE_LOG(LogTemp, Log, TEXT("[MonsterSpawnManager] Skip spawn (Control Room)"));
		return;
	}

	if (IsMonsterAlive())
	{
		UE_LOG(LogTemp, Log, TEXT("[MonsterSpawnManager] Skip spawn (Monster already alive)"));
		return;
	}

	if (!MonsterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MonsterSpawnManager] MonsterClass is NULL. Set it in Details."));
		return;
	}

	AMyPaperCharacter* Player = GetPlayer();
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MonsterSpawnManager] Player not found. Skip."));
		return;
	}

	const bool bSpawnFromLeft = FMath::RandBool();

	const float SpawnDistance = FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

	const FVector SpawnLoc = ComputeSpawnLocation(
		Player,
		bSpawnFromLeft,
		SpawnDistance
	);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMyPaperMonster* Monster = GetWorld()->SpawnActor<AMyPaperMonster>(
		MonsterClass,
		SpawnLoc,
		FRotator::ZeroRotator,
		Params
	);

	if (!Monster)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MonsterSpawnManager] Spawn failed."));
		return;
	}

	const float MoveDir = bSpawnFromLeft ? 1.f : -1.f;
	Monster->SetMoveDirectionX(MoveDir);

	Monster->InitTarget(Player, true, DetectRadius);

	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());

	if (GI && GI->SFXData && GI->SFXData->MonsterWalk)
	{
		Monster->StartWalkSound(GI->SFXData->MonsterWalk);
	}

	CurrentMonster = Monster;

	UE_LOG(LogTemp, Warning,
        TEXT("[MonsterSpawnManager] Spawned (%s) Dist=%.0f Reason=%s Loc=%s"),
        bSpawnFromLeft ? TEXT("Left->Right") : TEXT("Right->Left"),
        SpawnDistance,
        Reason == ESpawnReason::Random ? TEXT("Random") : TEXT("Event"),
        *SpawnLoc.ToString()
    );
}
