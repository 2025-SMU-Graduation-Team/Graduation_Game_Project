#include "DespawnVolume.h"
#include "Components/BoxComponent.h"
#include "MyPaperMonster.h"

ADespawnVolume::ADespawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Overlap ����
	Box->SetCollisionObjectType(ECC_WorldStatic); // ������ ���� ���� ����� �ڿ�������

	//Box->SetCollisionResponseToAllChannels(ECR_Ignore); // �⺻�� ���� ����
	//Box->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // ����(��κ� WorldDynamic)�� ���
	Box->SetCollisionResponseToAllChannels(ECR_Overlap);

	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // �÷��̾�� ����(����� ������ ����)
	Box->SetGenerateOverlapEvents(true);

	Box->SetBoxExtent(FVector(200.f, 200.f, 400.f)); // �⺻ ũ��(�������� �� ũ�� ���� ����)
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

	UE_LOG(LogTemp, Warning, TEXT("[DespawnVolume] Despawn monster: %s"), *GetNameSafe(Monster));
	Monster->Destroy();
}