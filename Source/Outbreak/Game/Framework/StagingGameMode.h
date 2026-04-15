// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StagingGameMode.generated.h" 

UCLASS()
class OUTBREAK_API AStagingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AStagingGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	void RequestStartGame();
	void ProcessPlayerQuit(APlayerController* ExitingPlayer);
protected:
	UPROPERTY(EditAnywhere, Category = "Config")
	TSoftObjectPtr<UWorld> TargetInGameLevel;
	
private:
	int32 ConnectedPlayers = 0;
	bool bIsTravelling = false;
	FTimerHandle TravelTimerHandle;
};