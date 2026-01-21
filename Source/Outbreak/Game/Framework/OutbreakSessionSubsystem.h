// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FindSessionsCallbackProxy.h"
#include "HttpModule.h"
#include "OutbreakSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOutbreakFindSessionsComplete, const TArray<FBlueprintSessionResult>&, SessionResults,bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutbreakCreateSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutbreakJoinSessionComplete,int32,bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutbreakDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutbreakSessionError, const FString&, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyMembersUpdated, const TArray<FString>&, Members);
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

	UFUNCTION(BlueprintCallable, Category = "Session")
	void HostGame(FString RoomCode, int32 MaxPlayers, bool bUseDedicatedServer);
	
	UFUNCTION(BlueprintCallable)
	void CreateSession(int32 NumPublicConnections, bool IsLAN);
	UFUNCTION(BlueprintCallable)
    void FindSessions(int32 MaxResults, FString RoomCode);
	UFUNCTION(BlueprintCallable)
	void JoinSession(const FBlueprintSessionResult& SessionResult);
	
	// Request via AWS Lambda 
	UFUNCTION(BlueprintCallable)
	void RequestGameSession();
	void RequestJoinTicket(FString SessionId, FString IP, int32 Port);
	TArray<FString> GetSteamLobbyMembers();
	FString GetCurrentRoomCode();
	bool IsSessionOwner();
	void OnSessionParticipantJoined(FName SessionName, const FUniqueNetId& UniqueId);

	FString GetAwsLambdaUrl() const;
private:
	// Callback Events
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsCompleted(bool bWasSuccessful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnSessionParticipantLeft(FName SessionName, const FUniqueNetId& UniqueId, EOnSessionParticipantLeftReason Reason);
	void OnJoinTicketReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void RefreshLobbyList();

	// AWS Callback
	void OnGameSessionResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void UpdateSteamLobby(FString IP, int32 Port, const FString& HostTicket,const FString& SessionId);
	void OnSteamLobbyUpdated(FName SessionName, const FOnlineSessionSettings& UpdatedSettings);

	void HandleNetworkFailure(UWorld* InWorld, UNetDriver* InNetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	bool IsLanEnvironment() const;
	bool IsValidSessionInterface();
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
	
	UPROPERTY(BlueprintAssignable)
	FOnOutbreakDestroySessionComplete OnDestroySessionComplete;

	UPROPERTY(BlueprintAssignable)
	FOnOutbreakSessionError OnSessionError;
	
	UPROPERTY(BlueprintAssignable)
	FOnLobbyMembersUpdated OnLobbyMembersUpdated;
	
private:
	IOnlineSessionPtr SessionInterface;
	
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	bool bIsLobbyHost = false;
	
	FString DesiredRoomCode;
	
	bool bLastRequestDedicated = false;

	FString SavedJoinIP;
	int32 SavedJoinPort;
	
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

};

