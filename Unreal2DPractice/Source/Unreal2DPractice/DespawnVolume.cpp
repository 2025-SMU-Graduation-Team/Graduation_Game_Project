#include "DespawnVolume.h"
#include "Components/BoxComponent.h"
#include "MyPaperMonster.h"

ADespawnVolume::ADespawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Overlap 전용
	Box->SetCollisionObjectType(ECC_WorldStatic); // 볼륨은 고정 지형 취급이 자연스러움

	//Box->SetCollisionResponseToAllChannels(ECR_Ignore); // 기본은 전부 무시
	//Box->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // 몬스터(대부분 WorldDynamic)만 잡기
	Box->SetCollisionResponseToAllChannels(ECR_Overlap);

	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // 플레이어는 무시(디버깅 노이즈 제거)
	Box->SetGenerateOverlapEvents(true);

	Box->SetBoxExtent(FVector(200.f, 200.f, 400.f)); // 기본 크기(레벨에서 더 크게 조정 가능)
}

void ADespawnVolume::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[DespawnVolume] BeginPlay: %s"), *GetName());

	if (Box)
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &ADespawnVolume::OnBoxBeginOverlap);
	}
}

void ADespawnVolume::OnBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("[DespawnVolume] fired: Actor=%s (%s) Comp=%s (%s) ObjType=%d"),
		*GetNameSafe(OtherActor),
		OtherActor ? *OtherActor->GetClass()->GetName() : TEXT("null"),
		*GetNameSafe(OtherComp),
		OtherComp ? *OtherComp->GetClass()->GetName() : TEXT("null"),
		OtherComp ? (int32)OtherComp->GetCollisionObjectType() : -1
	);

	AMyPaperMonster* Monster = Cast<AMyPaperMonster>(OtherActor);
	if (!Monster) return;

	//if (OtherComp != Monster->GetHitBox())
	//{g
	//	UE_LOG(LogTemp, Warning, TEXT("[DespawnVolume] Not Monster HitBox, ignore"));
	//	return;
	//}

	UE_LOG(LogTemp, Warning, TEXT("[DespawnVolume] Despawn monster: %s"), *GetNameSafe(Monster));
	Monster->Destroy();
}

