// Fill out your copyright notice in the Description page of Project Settings.


#include "NextLevelActor.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlayer.h"

void ANextLevelActor::GoToNextLevel()
{
	if (CutScene)
	{
		PlayCutScene();
	}
	else
	{
		MoveToNextLevel();
	}
}

void ANextLevelActor::PlayCutScene()
{
	ALevelSequenceActor* SeqActor;
	ULevelSequencePlayer* Player = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), CutScene, FMovieSceneSequencePlaybackSettings(), SeqActor);

	if (!Player) return;

	Player->OnFinished.AddDynamic(this, &ANextLevelActor::MoveToNextLevel);
	Player->Play();
}

void ANextLevelActor::MoveToNextLevel()
{
	if (NextLevelName.IsNone()) return;

	UGameplayStatics::OpenLevel(this, NextLevelName);

}
