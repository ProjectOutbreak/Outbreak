// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HttpModule.h"
#include "AwsSubsystem.generated.h"

UCLASS()
class OUTBREAK_API UAwsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Deinitialize() override;
	
	FString GetSteamId() const;
	FString GetSteamAuthTicket();
	
	void RequestGameSession();
	void RequestJoinTicket(FString SessionId, FString IP, int32 Port);
	TArray<FString> GetSteamLobbyMembers();

	FString GetAwsLambdaUrl() const;
	
private:
	void OnJoinTicketReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	void OnGameSessionResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	void UpdateSteamLobby(FString IP, int32 Port, const FString& HostTicket,const FString& SessionId);
	
	FString SavedJoinIP;
	int32 SavedJoinPort;
	TWeakPtr<IOnlineIdentity> SteamIdentityInterface;
	FName SteamName = FName("Steam");
};