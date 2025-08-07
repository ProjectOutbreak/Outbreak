// Fill out your copyright notice in the Description page of Project Settings.

#include "OBGameMode.h"
#include "OutBreakGameState.h"
#include "OutBreakPlayerState.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Game/Controller/OBPlayerController.h"
#include "Outbreak/Manager/CharacterSpawnManager.h"

AOBGameMode::AOBGameMode()
{
    DefaultPawnClass = ACharacterPlayer::StaticClass();
	PlayerControllerClass = AOBPlayerController::StaticClass();
	PlayerStateClass = AOutBreakPlayerState::StaticClass();
	GameStateClass = AOutBreakGameState::StaticClass();
}

void AOBGameMode::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnManager = GetWorld()->SpawnActor<ACharacterSpawnManager>(ACharacterSpawnManager::StaticClass(), SpawnParams);
}