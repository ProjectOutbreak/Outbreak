// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameLiftServerSDK.h"
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
public :
	
//-----Methods-----//

private:
	void OnStartGameSession(Aws::GameLift::Server::Model::GameSession GameSession);
	void OnProcessTerminate();
	bool OnHealthCheck();
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void StartGameServer();

	
	
};
