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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Logout(AController* Exiting) override;
	
	void OnPlayerDie(ACharacter* DeadCharacter, AController* Controller);
	void GameCleared();
	void GameOver();
	bool IsGameOver() const;

	void ProcessPlayerQuit(APlayerController* ExitingPlayer);
	void OnPlayerReady();
	
private:
	void InstantiateSpawnManager();
	void DelayedRefreshSpawnManagerTargets();
	void RefreshSpawnManagerTargets();

	UFUNCTION()
	void ProcessGameOverSequence();
	
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<AOutbreakSpectatorPawn> OutbreakSpectatorClass;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<ACharacterSpawnManager> SpawnManagerClass;
	UPROPERTY()
	TObjectPtr<ACharacterSpawnManager> SpawnManagerInstance;

	bool bHasGameOverTriggered = false;
	FTimerHandle GameOverTimerHandle;

	bool bIsServerShuttingDown = false;
	int32 ReadyPlayersCount = 0;
public:
	// ~ Begin Getter & Setter
	FORCEINLINE TObjectPtr<ACharacterSpawnManager> GetSpawnManager() const { return SpawnManagerInstance; }
	// ~ End Getter & Setter
};
