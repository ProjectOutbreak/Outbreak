// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "InGameMode.generated.h"

class AOutbreakSpectatorPawn;
class ACharacterPlayer;
class ACharacterSpawnManager;

UCLASS()
class OUTBREAK_API AInGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AInGameMode();
	virtual void BeginPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	void OnPlayerDie(ACharacter* DeadCharacter, AController* Controller);
	void ProceedToNextLevel() const;
	void GameOver();

private:
	void InstantiateSpawnManager();
	void DelayedRefreshSpawnManagerTargets();
	void RefreshSpawnManagerTargets();
	bool IsGameOver() const;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<AOutbreakSpectatorPawn> OutbreakSpectatorClass;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<ACharacterSpawnManager> SpawnManagerClass;
	UPROPERTY()
	TObjectPtr<ACharacterSpawnManager> SpawnManagerInstance;

	bool bHasGameOverTriggered = false;
public:
	// ~ Begin Getter & Setter
	FORCEINLINE TObjectPtr<ACharacterSpawnManager> GetSpawnManager() const { return SpawnManagerInstance; }
	// ~ End Getter & Setter
};
