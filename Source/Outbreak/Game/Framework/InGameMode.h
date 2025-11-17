// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "InGameMode.generated.h"

class ACharacterSpawnManager;

UCLASS()
class OUTBREAK_API AInGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AInGameMode();
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;

	UFUNCTION()
	void ProceedToNextLevel() const;

	FORCEINLINE TObjectPtr<class ACharacterSpawnManager> GetSpawnManager() const { return SpawnManager; }

protected:
	void ActivateSpawnManagerForPlayer(APlayerController* PlayerToTarget);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacterSpawnManager> SpawnManagerClass;
	UPROPERTY()
	TObjectPtr<ACharacterSpawnManager> SpawnManager;
};
