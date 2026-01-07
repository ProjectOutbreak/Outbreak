// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "InGameMode.generated.h"

class AOutbreakSpectatorPawn;
class ACharacterPlayer;
class ACharacterSpawnManager;

UCLASS()
class OUTBREAK_API AInGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AInGameMode();
	virtual void BeginPlay() override;
	
	virtual void OnPlayerDie(ACharacter* DeadCharacter, AController* Controller);

	UFUNCTION()
	void ProceedToNextLevel() const;

protected:
	void ActivateSpawnManagerForPlayer(APlayerController* PlayerToTarget);

private:
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AOutbreakSpectatorPawn> OutbreakSpectatorClass;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<ACharacterSpawnManager> SpawnManagerClass;
	UPROPERTY()
	TObjectPtr<ACharacterSpawnManager> SpawnManager;
	
public:
	// ~ Begin Getter & Setter
	FORCEINLINE TObjectPtr<ACharacterSpawnManager> GetSpawnManager() const { return SpawnManager; }
	// ~ End Getter & Setter
};
