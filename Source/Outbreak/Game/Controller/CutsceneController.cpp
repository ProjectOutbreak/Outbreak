// Fill out your copyright notice in the Description page of Project Settings.


#include "CutsceneController.h"
#include "Outbreak/UI/InGameHUD.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Game/Framework/InGameState.h"

void UCutsceneController::Init(UWorld* InWorld)
{
	WorldRef = InWorld;
}

void UCutsceneController::PlayCutscene(ULevelSequence* Sequence)
{
	if (!WorldRef || !Sequence) return;
	
	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bAutoPlay = false;

	ALevelSequenceActor* OutActor = nullptr;
	ULevelSequencePlayer* Player = ULevelSequencePlayer::CreateLevelSequencePlayer(WorldRef, Sequence, Settings, OutActor);
	if (Player)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldRef, 0))
		{
			if (AInGameHUD* HUD = Cast<AInGameHUD>(PC->GetHUD()))
			{
				HUD->SetCutsceneMode(true);
			}
		}
		APlayerController* PC = UGameplayStatics::GetPlayerController(WorldRef, 0);
		if (PC && PC->IsLocalController())
		{
			PC->DisableInput(PC);
			if (APawn* Pawn = PC->GetPawn())
			{
				if (ACharacter* Character = Cast<ACharacter>(Pawn))
				{
					Character->GetCharacterMovement()->DisableMovement();
					if (ACharacterPlayer* CP = Cast<ACharacterPlayer>(Character))
					{
						CP->SetIsCutscenePlaying(true);
					}
				}
			}
		}
		Player->OnFinished.AddDynamic(this, &UCutsceneController::OnCutSceneFinished);
		Player->Play();
	}
}

void UCutsceneController::OnCutSceneFinished()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(WorldRef, 0);
	if (PC && PC->IsLocalController())
	{
		PC->EnableInput(PC);
		if (APawn* Pawn = PC->GetPawn())
		{
			if (ACharacter* Character = Cast<ACharacter>(Pawn))
			{
				Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				if (ACharacterPlayer* CP = Cast<ACharacterPlayer>(Character))
				{
					CP->SetIsCutscenePlaying(false);
				}
			}
		}
	}

	if (APlayerController* PC2 = UGameplayStatics::GetPlayerController(WorldRef, 0))
	{
		if (AInGameHUD* HUD = Cast<AInGameHUD>(PC2->GetHUD()))
		{
			HUD->SetCutsceneMode(false);
		}
	}
}
