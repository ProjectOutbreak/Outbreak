// Fill out your copyright notice in the Description page of Project Settings.


#include "OutbreakSessionSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "OutbreakAuthSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"
THIRD_PARTY_INCLUDES_START
#include "aws/core/Aws.h"
#include "aws/gamelift/GameLiftClient.h"
#include "aws/gamelift/model/StartGameSessionPlacementRequest.h"
#include "aws/gamelift/model/DescribeGameSessionPlacementRequest.h"
#include "aws/gamelift/model/GameSessionPlacementState.h"
#include "aws/gamelift/model/DesiredPlayerSession.h"
THIRD_PARTY_INCLUDES_END

UOutbreakSessionSubsystem::UOutbreakSessionSubsystem() :m_SdkOptions(nullptr), m_GameLiftClient(nullptr) {}
UOutbreakSessionSubsystem::~UOutbreakSessionSubsystem() { ShutdownAwsSDK(); }

// =============== Init / Deinitializing ===============
void UOutbreakSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (!IsRunningDedicatedServer())
		InitAwsSDK();
	
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("[Steam] Initializing Steam Session"));
		}
	}
	SessionInterface->AddOnSessionSettingsUpdatedDelegate_Handle(
			FOnSessionSettingsUpdatedDelegate::CreateUObject(this, &UOutbreakSessionSubsystem::OnSteamLobbyUpdated));
}
void UOutbreakSessionSubsystem::Deinitialize()
{
	ShutdownAwsSDK();
	Super::Deinitialize();

}
void UOutbreakSessionSubsystem::InitAwsSDK()
{
	m_SdkOptions = new Aws::SDKOptions;
	Aws::InitAPI(*m_SdkOptions);

	Aws::Client::ClientConfiguration ClientConfig;
	ClientConfig.region = "ap-northeast-2";
	
	// TODO : ADD Credentials 
	m_GameLiftClient = new Aws::GameLift::GameLiftClient(ClientConfig);

	UE_LOG(LogTemp, Log, TEXT("[AWS Gamelift] AWS SDK initialized"));
}
void UOutbreakSessionSubsystem::ShutdownAwsSDK()
{
	if (m_GameLiftClient) delete m_GameLiftClient; m_GameLiftClient = nullptr;
	if (m_SdkOptions) delete m_SdkOptions; m_SdkOptions = nullptr;
}

// =============== GameLift Request & Response ===============
void UOutbreakSessionSubsystem::RequestGameSession()
{
	if (!m_GameLiftClient) return;
	
	Aws::GameLift::Model::StartGameSessionPlacementRequest request;
	FString NewId = FGuid::NewGuid().ToString();
	CurrentPlacementId = NewId;
	request.SetPlacementId(TCHAR_TO_UTF8(*CurrentPlacementId));
	// -----------------------DO NOT COMMIT ON GIT ------------------------------------
	request.SetGameSessionQueueName("");
	request.SetMaximumPlayerSessionCount(4);

	// Gamelift에 세션멤버 전부 등록
	TArray<FString> TeamMembers = GetSteamLobbyMembers();
	for (const FString& MemberID : TeamMembers)
	{
		Aws::GameLift::Model::DesiredPlayerSession playerSession;
		playerSession.SetPlayerId(TCHAR_TO_UTF8(*MemberID));
		request.AddDesiredPlayerSessions(playerSession);
	}
	// request success condition
	auto outcome = m_GameLiftClient->StartGameSessionPlacement(request);
	if (outcome.IsSuccess())
	{
		UE_LOG(LogTemp,Warning,TEXT("[Session] request success... Polling Started..."));
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(PollingTimerHandle,this, &UOutbreakSessionSubsystem::OnPlacementStatusCheck,1.0f,true);
			// broadcast delegate
			OnStartGameSessionResult.Broadcast(true);
		}
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("[Session] request failed. ID : %s"),UTF8_TO_TCHAR(outcome.GetError().GetMessage().c_str()));	
		OnStartGameSessionResult.Broadcast(false);

	}
}
void UOutbreakSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!SessionInterface.IsValid()) return;
	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
	if (Success)
	{
		// call Server Travel
		UE_LOG(LogTemp, Warning, TEXT("[Steam] OnCreateSessionComplete Success"));
		OnCreateLobbyResult.Broadcast(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Steam] OnCreateSessionComplete Failed"));
		OnCreateLobbyResult.Broadcast(false);
	}
}
void UOutbreakSessionSubsystem::OnPlacementStatusCheck()
{
	if (!m_GameLiftClient) return;
	Aws::GameLift::Model::DescribeGameSessionPlacementRequest request;
	request.SetPlacementId(TCHAR_TO_UTF8(*CurrentPlacementId));

	auto outcome = m_GameLiftClient->DescribeGameSessionPlacement(request);
	if (outcome.IsSuccess())
	{
		auto placement = outcome.GetResult().GetGameSessionPlacement();
		auto status = placement.GetStatus();
		if (status == Aws::GameLift::Model::GameSessionPlacementState::FULFILLED)
		{
			// 성공! 타이머 정지
			if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(PollingTimerHandle);

			FString IP = UTF8_TO_TCHAR(placement.GetIpAddress().c_str());
			int32 Port = placement.GetPort();
            
			TMap<FString, FString> TicketMap;
			for (const auto& PS : placement.GetPlacedPlayerSessions())
			{
				FString P_ID = UTF8_TO_TCHAR(PS.GetPlayerId().c_str());
				FString P_Ticket = UTF8_TO_TCHAR(PS.GetPlayerSessionId().c_str());
				TicketMap.Add(P_ID, P_Ticket);
			}
			UE_LOG(LogTemp, Warning, TEXT("[AWS] MATCH FOUND! Broadcasting info to Steam Lobby..."));
			UpdateSteamLobby(IP, Port, TicketMap);
		}
		else if (status == Aws::GameLift::Model::GameSessionPlacementState::TIMED_OUT ||
				 status == Aws::GameLift::Model::GameSessionPlacementState::FAILED)
		{
			if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(PollingTimerHandle);
			UE_LOG(LogTemp, Error, TEXT("[AWS] Placement Failed/TimedOut."));
		}
	}
}

void UOutbreakSessionSubsystem::OnSteamLobbyUpdated(FName SessionName, const FOnlineSessionSettings& UpdatedSettings)
{
	//SteamLobby에 AWS IP가 등록되면 실행할 콜백.
	FString ServerIP;
    if (!UpdatedSettings.Get(FName("SERVER_IP"), ServerIP)) return;
	int32 ServerPort = 0;
	UpdatedSettings.Get(FName("SERVER_PORT"), ServerPort);
	
	// Find Own Steam ID
	FString SteamId;
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem && Subsystem->GetIdentityInterface())
	{
		if (Subsystem->GetIdentityInterface()->GetUniquePlayerId(0).IsValid())
		{
			SteamId = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0)->ToString();
		}
	}
	if (SteamId.IsEmpty()) 
	{
		UE_LOG(LogTemp, Error, TEXT("[OnSteamLobbyUpdated] Steam ID Not Found "));
		return;
	}
	
	FString TicketKey = FString::Printf(TEXT("TICKET_%s"), *SteamId);
	FString Ticket = "";

	if (UpdatedSettings.Get(FName(*TicketKey), Ticket))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnSteamLobbyUpdated] Found GameLift Ticket! Traveling to Dedicated Server..."));
                
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            FString Url = FString::Printf(TEXT("%s:%d?PlayerSessionId=%s"), *ServerIP, ServerPort, *Ticket);
            PC->ClientTravel(Url, TRAVEL_Absolute);
        }
	}
	else
	{
        UE_LOG(LogTemp, Warning, TEXT("[OnSteamLobbyUpdated] Found NO TICKET for me (%s). Waiting..."), *SteamId);
	}
}

// ===============  Steam Lobby =============== 
void UOutbreakSessionSubsystem::CreateSteamLobby(int32 MaxPlayers)
{
	if (!SessionInterface.IsValid()) return;
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}
	
	CreateSessionDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UOutbreakSessionSubsystem::OnCreateSessionComplete));

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

	// setting of steamLobby 
	SessionSettings->bIsLANMatch = false;                 
	SessionSettings->NumPublicConnections = MaxPlayers;   
	SessionSettings->bAllowJoinInProgress = true;          
	SessionSettings->bAllowJoinViaPresence = true;         
	SessionSettings->bShouldAdvertise = true;             
	SessionSettings->bUsesPresence = true;                 
	SessionSettings->bUseLobbiesIfAvailable = true;        

	SessionSettings->Set(SETTING_MAPNAME, FString("L_Lobby"), EOnlineDataAdvertisementType::ViaOnlineService);

	// set room key
	const int32 CodeLength = 6;
	const FString CharSet = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	FString RoomCode = TEXT("");

	for (int32 i = 0; i < CodeLength; i++)
	{
		int32 Index = FMath::RandRange(0,CharSet.Len() - 1);
		RoomCode += CharSet.Mid(Index,1);
	}
	
	SessionSettings->Set(FName("ROOM_CODE"), RoomCode, EOnlineDataAdvertisementType::ViaOnlineService);
	
	UE_LOG(LogTemp, Warning, TEXT("[Steam] Creating Session..."));
	if (!SessionInterface->CreateSession(0, NAME_GameSession, *SessionSettings))
	{
		UE_LOG(LogTemp, Error, TEXT("[Steam] Failed to call CreateSession"));
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
	}
}
void UOutbreakSessionSubsystem::JoinSteamLobby(FString RoomCode)
{
	if (!SessionInterface.IsValid()) return;

	TargetRoomCode = RoomCode;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 1000;

	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);
	//SessionSearch->QuerySettings.Set(FName("ROOM_CODE"), RoomCode, EOnlineComparisonOp::Equals);

	// subscribe delegate
	FindSessionDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this,&UOutbreakSessionSubsystem::OnFindSessionResult));
	
	// call FindSession
	UE_LOG(LogTemp, Warning, TEXT("[Session] Searching for Lobby with Code: %s..."), *RoomCode);
	SessionInterface->FindSessions(0,SessionSearch.ToSharedRef());
}
void UOutbreakSessionSubsystem::OnFindSessionResult(bool bSucceeded)
{
	if (!SessionInterface.IsValid()) return;
	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionDelegateHandle);

	if (!bSucceeded || !SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Session] Session Search failed."));
		// call UI Delegate (session failed)
		return;
	}

	const FOnlineSessionSearchResult* TargetResult = nullptr;

	for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
	{
		if (!Result.IsValid()) continue;
		FString ServerRoomCode;
		if (Result.Session.SessionSettings.Get(ROOM_CODE_KEY,ServerRoomCode))
		{
			if (ServerRoomCode == TargetRoomCode)
			{
				TargetResult = &Result;
				break;
			}
		}
	}

	if (TargetResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Session] Code Match! Joining Session..."));
		
		FOnlineSessionSearchResult SearchResultToJoin = *TargetResult;
		SearchResultToJoin.Session.SessionSettings.bUsesPresence = true;
		SearchResultToJoin.Session.SessionSettings.bUseLobbiesIfAvailable = true;
		
		JoinSessionDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateUObject(this, &UOutbreakSessionSubsystem::OnJoinSessionComplete));

		SessionInterface->JoinSession(0, NAME_GameSession, SearchResultToJoin);	} 
}
void UOutbreakSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString ConnectInfo;
		if (SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectInfo))
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
          
			if (PC)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Session] Traveling to: %s"), *ConnectInfo);
				PC->ClientTravel(ConnectInfo, TRAVEL_Absolute);
			}
		}
		OnJoinLobbyResult.Broadcast(true);	}
	else
	{
		OnJoinLobbyResult.Broadcast(false);
	}
}
void UOutbreakSessionSubsystem::UpdateSteamLobby(FString IP, int32 Port, TMap<FString, FString> PlayerTickets)
{
	if (!SessionInterface.IsValid()) return;
	FOnlineSessionSettings* SessionSettings = SessionInterface->GetSessionSettings(NAME_GameSession);

	if (!SessionSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("[UpdateSteamLobby] Failed to Get SessionSettings. "));
		return;
	}

	SessionSettings->Set(FName("SERVER_IP"),IP,EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings->Set(FName("SERVER_PORT"),Port,EOnlineDataAdvertisementType::ViaOnlineService);
	
	for (const auto& Entry:PlayerTickets)
	{
		FString key = FString::Printf(TEXT("Ticket_%s_%s"), *Entry.Key);
		SessionSettings->Set(FName(*key),Entry.Value,EOnlineDataAdvertisementType::ViaOnlineService);
		UE_LOG(LogTemp, Log, TEXT("[UpdateSteamLobby] Posting Ticket for %s"), *Entry.Key);
	}
	UE_LOG(LogTemp, Warning, TEXT("[UpdateSteamLobby] Broadcasting GameLift Info to Lobby Members..."));
	SessionInterface->UpdateSession(NAME_GameSession, *SessionSettings, true);
}


TArray<FString> UOutbreakSessionSubsystem::GetSteamLobbyMembers()
{
	TArray<FString> LobbyMembers;
	if (!SessionInterface.IsValid()) return LobbyMembers;
	FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession);

	if (Session)
	{
		for (const auto& PlayerID : Session->RegisteredPlayers)
		{
			LobbyMembers.Add(PlayerID->ToString());
		}
	}
	return LobbyMembers;
}


