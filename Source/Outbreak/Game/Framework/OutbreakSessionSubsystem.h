// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FindSessionsCallbackProxy.h"
#include "HttpModule.h"
#include "OutbreakSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutbreakFindSessionsComplete, const TArray<FBlueprintSessionResult>&, SessionResults);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutbreakCreateSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutbreakJoinSessionComplete,bool,bWasSuccessful);

UCLASS()
class OUTBREAK_API UOutbreakSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
// --------------------
// Functions
// --------------------
public:
	// ---------- Methods ---------- //
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	void CreateSession(int32 MaxPlayers = 4 , bool IsLAN = false, FString RoomCode);
	UFUNCTION(BlueprintCallable)
	void JoinSession(const FBlueprintSessionResult& SessionResult);
	UFUNCTION(BlueprintCallable)
	void FindSessions(int32 MaxResults, bool bIsLAN);
	
	// Request via AWS Lambda 
	UFUNCTION(BlueprintCallable)
	void RequestGameSession();

private:
	// Callback Events
	void OnSteamLobbyUpdated(FName SessionName, const FOnlineSessionSettings& UpdatedSettings);
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsCompleted(bool bWasSuccessful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	// Http Callback of RequestGameSession 
	void OnGameSessionResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void UpdateSteamLobby(FString IP, int32 Port, TMap<FString, FString> PlayerTickets);
	TArray<FString> GetSteamLobbyMembers();
	bool IsSessionOwner();

// --------------------
// Variables
// --------------------
public:
	// ---------- Delegates ---------- //
	UPROPERTY(BlueprintAssignable)
	FOnOutbreakFindSessionsComplete OnFindSessionsComplete;
	UPROPERTY(BlueprintAssignable)
	FOnOutbreakCreateSessionComplete OnCreateSessionComplete;
	UPROPERTY(BlueprintAssignable)
	FOnOutbreakJoinSessionComplete OnJoinSessionComplete;
	
private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
};

