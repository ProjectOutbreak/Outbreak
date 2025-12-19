// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"

namespace Aws {struct SDKOptions;}
namespace Aws {namespace GameLift {class GameLiftClient;}}

#include "OutbreakSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateLobbyResult, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinLobbyResult, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartGameSessionResult,bool,bWasSuccessful);

UCLASS()
class OUTBREAK_API UOutbreakSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	//-----Variables-----//

	// Init
	Aws::SDKOptions* m_SdkOptions;
	Aws::GameLift::GameLiftClient* m_GameLiftClient;
	IOnlineSessionPtr SessionInterface;
	
	FTimerHandle PollingTimerHandle;
	FString CurrentPlacementId;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	FString TargetRoomCode;
	const FName ROOM_CODE_KEY = FName("ROOM_CODE");

	// Delegate 
	FDelegateHandle FindSessionDelegateHandle;
	FDelegateHandle JoinSessionDelegateHandle;
	FDelegateHandle CreateSessionDelegateHandle;

public:
	UPROPERTY(BlueprintAssignable)
	FOnCreateLobbyResult OnCreateLobbyResult;
	UPROPERTY(BlueprintAssignable)
	FOnJoinLobbyResult OnJoinLobbyResult;
	UPROPERTY(BlueprintAssignable)
	FOnStartGameSessionResult OnStartGameSessionResult;
	
private:
	//-----Function-----//
	void InitAwsSDK();
	void ShutdownAwsSDK();
	void OnSteamLobbyUpdated(FName SessionName, const FOnlineSessionSettings& UpdatedSettings);

	
protected:
	void OnFindSessionResult(bool bSucceeded);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	TArray<FString> GetSteamLobbyMembers();

public:
	//-----Function-----//
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
	UOutbreakSessionSubsystem();
	~UOutbreakSessionSubsystem();
	
	UFUNCTION(BlueprintCallable)
	void RequestGameSession();
	UFUNCTION(BlueprintCallable)
	void CreateSteamLobby(int32 MaxPlayers);
	UFUNCTION(BlueprintCallable)
	void JoinSteamLobby(FString RoomCode);

	void OnPlacementStatusCheck();
	void UpdateSteamLobby(FString IP, int32 Port, TMap<FString, FString> PlayerTickets);
	void OnCreateSessionComplete(FName SessionName, bool Success);
	
};

