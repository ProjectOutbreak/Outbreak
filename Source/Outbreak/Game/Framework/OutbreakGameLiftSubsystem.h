// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameLiftServerSDK.h"
#include <atomic>
#include "OutbreakGameLiftSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class OUTBREAK_API UOutbreakGameLiftSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

//-----Variables-----//
private :
	FGameLiftServerSDKModule* GameLiftSdkModule;
	bool bIsInitialized = false;
	bool bProcessEndingInitiated = false;
	FDelegateHandle OnWorldLoadedDelegateHandle;
	std::atomic<bool> bIsServerHealthy;
//-----Methods-----//
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void StartGameServer();
	void EndGameServer();
	void OnWorldLoaded(UWorld* World);
	void TriggerProcessEnding();

private:
	void OnStartGameSession(Aws::GameLift::Server::Model::GameSession GameSession);
	void OnProcessTerminate();
	bool OnHealthCheck();
	
};
