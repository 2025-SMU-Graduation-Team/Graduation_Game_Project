#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"
#include "GameSFXData.h"
#include "GameBGMData.h"

AAudioManager::AAudioManager()
{
	PrimaryActorTick.bCanEverTick = false;

	BGMComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComponent"));
	RootComponent = BGMComponent;
	BGMComponent->bAutoActivate = false;
}

void AAudioManager::BeginPlay()
{
	Super::BeginPlay();

	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());

	if (GI)
	{
		SFXData = GI->SFXData;
		BGMData = GI->BGMData;
	}
}

void AAudioManager::PlaySFX(USoundBase* Sound, FVector Location)
{
	if (!Sound) return;

	UGameplayStatics::PlaySoundAtLocation(
		this,
		Sound,
		Location
	);
}

void AAudioManager::PlaySFX2D(USoundBase* Sound)
{
	if (!Sound) return;

	UGameplayStatics::PlaySound2D(
		this,
		Sound
	);
}

void AAudioManager::PlayBGM(USoundBase* Sound)
{
	if (!Sound) return;

	BGMComponent->SetSound(Sound);
	BGMComponent->Play();
}

void AAudioManager::StopBGM()
{
	BGMComponent->Stop();
}