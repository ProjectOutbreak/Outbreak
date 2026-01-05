#include "OutbreakSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"
#include "OutbreakAuthSubsystem.h"
#include "Interfaces/IHttpResponse.h"

UOutbreakSessionSubsystem::UOutbreakSessionSubsystem() {}

void UOutbreakSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (IsRunningDedicatedServer()) return;
    
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UOutbreakSessionSubsystem::OnCreateSessionCompleted);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UOutbreakSessionSubsystem::OnFindSessionsCompleted);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UOutbreakSessionSubsystem::OnJoinSessionCompleted);
            
            SessionInterface->AddOnSessionSettingsUpdatedDelegate_Handle(
                FOnSessionSettingsUpdatedDelegate::CreateUObject(this, &UOutbreakSessionSubsystem::OnSteamLobbyUpdated));
        }
    }
}

void UOutbreakSessionSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UOutbreakSessionSubsystem::CreateSession(FString RoomCode, int32 MaxPlayers, bool IsLAN)
{
    if (!SessionInterface.IsValid()) 
    {
        OnCreateSessionComplete.Broadcast(false);
        return;
    }

    auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
    if (ExistingSession != nullptr)
    {
        SessionInterface->DestroySession(NAME_GameSession);
    }

    TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

    SessionSettings->bIsLANMatch = IsLAN;
    SessionSettings->NumPublicConnections = MaxPlayers;
    SessionSettings->bAllowJoinInProgress = true;
    SessionSettings->bAllowJoinViaPresence = true;
    SessionSettings->bShouldAdvertise = true;
    SessionSettings->bUsesPresence = true;
    SessionSettings->bUseLobbiesIfAvailable = true;

    SessionSettings->Set(SETTING_MAPNAME, FString("L_Lobby"), EOnlineDataAdvertisementType::ViaOnlineService);
    SessionSettings->Set(FName("GAME_ID"), FString("OUTBREAK"), EOnlineDataAdvertisementType::ViaOnlineService);
    
    if (RoomCode.IsEmpty()) RoomCode = TEXT("PUBLIC");
    SessionSettings->Set(FName("ROOM_CODE"), RoomCode, EOnlineDataAdvertisementType::ViaOnlineService);

    UE_LOG(LogTemp, Log, TEXT("[Session] Creating Session... Code: %s"), *RoomCode);
    
    if (!SessionInterface->CreateSession(0, NAME_GameSession, *SessionSettings))
    {
        OnCreateSessionComplete.Broadcast(false);
    }
}

void UOutbreakSessionSubsystem::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("[Session] Success! Opening Lobby Level..."));
        UGameplayStatics::OpenLevel(GetWorld(), "L_Lobby", true, "listen");
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Session] Create Session Failed!"));
    }
    OnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UOutbreakSessionSubsystem::FindSessions(int32 MaxResults, bool IsLAN)
{
    if (!SessionInterface.IsValid()) 
    {
        OnFindSessionsComplete.Broadcast(TArray<FBlueprintSessionResult>());
        return;
    }

    LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
    LastSessionSearch->MaxSearchResults = MaxResults;
    LastSessionSearch->bIsLanQuery = IsLAN;
    LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); 

    UE_LOG(LogTemp, Log, TEXT("[Session] Searching for sessions..."));
    SessionInterface->FindSessions(0, LastSessionSearch.ToSharedRef());
}

void UOutbreakSessionSubsystem::OnFindSessionsCompleted(bool bWasSuccessful)
{
    TArray<FBlueprintSessionResult> Results;

    if (bWasSuccessful && LastSessionSearch.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("[Session] Found %d sessions."), LastSessionSearch->SearchResults.Num());

        for (const FOnlineSessionSearchResult& Result : LastSessionSearch->SearchResults)
        {
            if (Result.IsValid())
            {
                FBlueprintSessionResult BPResult;
                BPResult.OnlineResult = Result;
                Results.Add(BPResult);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Session] No sessions found or Failed."));
    }

    OnFindSessionsComplete.Broadcast(Results);
}

void UOutbreakSessionSubsystem::JoinSession(const FBlueprintSessionResult& SessionResult)
{
    if (!SessionInterface.IsValid())
    {
        OnJoinSessionComplete.Broadcast(false);
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("[Session] Joining Session..."));
    SessionInterface->JoinSession(0, NAME_GameSession, SessionResult.OnlineResult);
}

void UOutbreakSessionSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        UE_LOG(LogTemp, Log, TEXT("[Session] Join Success! Client Travel Start..."));
        
        FString ConnectInfo;
        if (SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectInfo))
        {
            if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
            {
                PC->ClientTravel(ConnectInfo, TRAVEL_Absolute);
            }
        }
        OnJoinSessionComplete.Broadcast(true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Session] Join Failed."));
        OnJoinSessionComplete.Broadcast(false);
    }
}

void UOutbreakSessionSubsystem::RequestGameSession()
{
    if (!IsSessionOwner()) 
    {
       UE_LOG(LogTemp, Warning, TEXT("[Client] Only Host can request GameSession."));
       return;
    }
    
    TArray<FString> LobbyMembers = GetSteamLobbyMembers();
    if (LobbyMembers.Num() == 0) return;
    
    UOutbreakAuthSubsystem* AuthSys = GetGameInstance()->GetSubsystem<UOutbreakAuthSubsystem>();
    if (!AuthSys) return;
    
    FString ticket = AuthSys->GetSteamAuthTicket();
    if (ticket.IsEmpty()) return;
    
    TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
    RequestObj->SetStringField("ticket", ticket);
    RequestObj->SetStringField("SteamId", AuthSys->GetSteamId());
    
    TArray<TSharedPtr<FJsonValue>> JsonMembers;
    for (const FString& MemberID : LobbyMembers)
    {
       JsonMembers.Add(MakeShareable(new FJsonValueString(MemberID)));
    }
    RequestObj->SetArrayField("player_ids", JsonMembers);

    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer);

    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
    
    // Lambda URL
    HttpRequest->SetURL("https://ike2oniii4.execute-api.ap-northeast-2.amazonaws.com/default/OutbreakGameSessionRequest");
    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader("Content-Type", "application/json");
    HttpRequest->SetContentAsString(RequestBody);
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UOutbreakSessionSubsystem::OnGameSessionResponseReceived);

    UE_LOG(LogTemp, Log, TEXT("[AWS] Requesting GameSession..."));
    HttpRequest->ProcessRequest();
}

void UOutbreakSessionSubsystem::OnGameSessionResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
    {
       UE_LOG(LogTemp, Error, TEXT("[AWS] HTTP Request Failed!"));
       return;
    }
    
    FString ResponseStr = Response->GetContentAsString();
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseStr);
    
    if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
    {
       if (JsonObject->HasField("errorMessage")) return;
       
       FString ServerIP = JsonObject->GetStringField(TEXT("ip_address"));
       if (ServerIP.IsEmpty()) ServerIP = JsonObject->GetStringField(TEXT("ip"));
       int32 ServerPort = JsonObject->GetNumberField(TEXT("port"));
       
       TMap<FString, FString> TicketMap;
       const TSharedPtr<FJsonObject>* TicketsJson;
       if (JsonObject->TryGetObjectField(TEXT("tickets"), TicketsJson))
       {
          for (auto CurrJsonValue = (*TicketsJson)->Values.CreateConstIterator(); CurrJsonValue; ++CurrJsonValue)
          {
             TicketMap.Add(CurrJsonValue->Key, CurrJsonValue->Value->AsString());
          }
       }
       
       if (!ServerIP.IsEmpty() && ServerPort > 0)
       {
          UpdateSteamLobby(ServerIP, ServerPort, TicketMap);
       }
    }
}

void UOutbreakSessionSubsystem::UpdateSteamLobby(FString IP, int32 Port, TMap<FString, FString> PlayerTickets)
{
    if (!SessionInterface.IsValid()) return;
    FOnlineSessionSettings* SessionSettings = SessionInterface->GetSessionSettings(NAME_GameSession);
    if (!SessionSettings) return;

    SessionSettings->Set(FName("SERVER_IP"), IP, EOnlineDataAdvertisementType::ViaOnlineService);
    SessionSettings->Set(FName("SERVER_PORT"), Port, EOnlineDataAdvertisementType::ViaOnlineService);
    
    for (const auto& Entry : PlayerTickets)
    {
       FString key = FString::Printf(TEXT("TICKET_%s"), *Entry.Key);
       SessionSettings->Set(FName(*key), Entry.Value, EOnlineDataAdvertisementType::ViaOnlineService);
    }
    
    SessionInterface->UpdateSession(NAME_GameSession, *SessionSettings, true);
}

void UOutbreakSessionSubsystem::OnSteamLobbyUpdated(FName SessionName, const FOnlineSessionSettings& UpdatedSettings)
{
    FString ServerIP;
    if (!UpdatedSettings.Get(FName("SERVER_IP"), ServerIP)) return;
    
    int32 ServerPort = 0;
    UpdatedSettings.Get(FName("SERVER_PORT"), ServerPort);
    
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(TEXT("STEAM"));
    FString SteamId;
    if (Subsystem && Subsystem->GetIdentityInterface())
    {
        auto UserId = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
        if (UserId.IsValid()) SteamId = UserId->ToString();
    }
    
    if (SteamId.IsEmpty()) return;
    
    FString TicketKey = FString::Printf(TEXT("TICKET_%s"), *SteamId);
    FString Ticket;

    if (UpdatedSettings.Get(FName(*TicketKey), Ticket))
    {
        UE_LOG(LogTemp, Warning, TEXT("[Steam] Found Ticket! Moving to Dedicated Server..."));
        if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
        {
            FString Url = FString::Printf(TEXT("%s:%d?PlayerSessionId=%s"), *ServerIP, ServerPort, *Ticket);
            PC->ClientTravel(Url, TRAVEL_Absolute);
        }
    }
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
           if (PlayerID->IsValid()) LobbyMembers.Add(PlayerID->ToString());
       }
       if (LobbyMembers.Num() == 0)
       {
           IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(TEXT("STEAM"));
           if (Subsystem && Subsystem->GetIdentityInterface())
           {
               auto Id = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
               if (Id.IsValid()) LobbyMembers.Add(Id->ToString());
           }
       }
    }
    return LobbyMembers;
}

bool UOutbreakSessionSubsystem::IsSessionOwner()
{
    if (!SessionInterface.IsValid()) return false;
    FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession);
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(TEXT("STEAM"));
    if (!Session || !Subsystem) return false;
    
    auto MyId = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
    return (Session->OwningUserId.IsValid() && MyId.IsValid() && *Session->OwningUserId == *MyId);
}