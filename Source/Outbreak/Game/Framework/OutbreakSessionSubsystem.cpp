#include "OutbreakSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"
#include "OutbreakAuthSubsystem.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/ConfigCacheIni.h"

const FName KEY_ROOM_CODE(TEXT("ROOM_CODE"));
const FName KEY_GAME_ID(TEXT("GAME_ID"));
const FString GAME_ID_VALUE(TEXT("OUTBREAK"));

void UOutbreakSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (GEngine)
    {
        GEngine->OnNetworkFailure().AddUObject(this, &UOutbreakSessionSubsystem::HandleNetworkFailure);
    }
    
    if (IsRunningDedicatedServer()) return;
    
    if (const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
    {
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnSessionParticipantLeftDelegates.AddUObject(this, &UOutbreakSessionSubsystem::OnSessionParticipantLeft);
            SessionInterface->OnSessionParticipantJoinedDelegates.AddUObject(this, &UOutbreakSessionSubsystem::OnSessionParticipantJoined);

            SessionInterface->AddOnSessionSettingsUpdatedDelegate_Handle(
                FOnSessionSettingsUpdatedDelegate::CreateUObject(this, &UOutbreakSessionSubsystem::OnSteamLobbyUpdated));
        }
    }
}

void UOutbreakSessionSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UOutbreakSessionSubsystem::HostGame(FString RoomCode, int32 MaxPlayers, bool bUseDedicatedServer)
{
    if (RoomCode.IsEmpty()) RoomCode = TEXT("PUBLIC");
    DesiredRoomCode = RoomCode;
    bLastRequestDedicated = bUseDedicatedServer; 

    auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
    if (ExistingSession != nullptr)
    {
        SessionInterface->DestroySession(NAME_GameSession);
    }

    CreateSession(MaxPlayers, IsLanEnvironment());
}

void UOutbreakSessionSubsystem::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
    }
    OnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UOutbreakSessionSubsystem::HandleNetworkFailure(UWorld* InWorld, UNetDriver* InNetDriver,
    ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    OnSessionError.Broadcast(FString::Printf(TEXT("Network Failure: %s"), *ErrorString));
}

bool UOutbreakSessionSubsystem::IsLanEnvironment() const
{
#if WITH_EDITOR
    if (FParse::Param(FCommandLine::Get(), TEXT("nosteam")) ||
        FParse::Param(FCommandLine::Get(), TEXT("tr_lan")))
    {
        return true;
    }
#endif
    return IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
}

bool UOutbreakSessionSubsystem::IsValidSessionInterface()
{
    if (!SessionInterface.IsValid())
    {
        if (const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
        {
            SessionInterface = Subsystem->GetSessionInterface();
        }
    }
    return SessionInterface.IsValid();
}

void UOutbreakSessionSubsystem::CreateSession(int32 NumPublicConnections, bool IsLAN)
{
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[CreateSession] Session Interface is Invalid!"));
        OnSessionError.Broadcast(TEXT("Session Is Not Valid"));
        return;
    }
    CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UOutbreakSessionSubsystem::OnCreateSessionCompleted));
    
    TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

    SessionSettings->bIsLANMatch = IsLAN;
    SessionSettings->NumPublicConnections = NumPublicConnections;
    SessionSettings->bAllowJoinInProgress = true;
    SessionSettings->bAllowJoinViaPresence = true;
    SessionSettings->bShouldAdvertise = true;
    SessionSettings->bUsesPresence = true;
    SessionSettings->bUseLobbiesIfAvailable = true;
    
    SessionSettings->Set(FName("ROOM_CODE"), DesiredRoomCode, EOnlineDataAdvertisementType::ViaOnlineService);
    SessionSettings->Set(FName("GAME_ID"), FString("OUTBREAK"), EOnlineDataAdvertisementType::ViaOnlineService);
    UE_LOG(LogTemp, Log, TEXT("[HostGame] Creating Session... Code: %s, Dedicated: %d"), *DesiredRoomCode, bLastRequestDedicated);    

    if (!SessionInterface->CreateSession(0, NAME_GameSession, *SessionSettings))
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
        OnCreateSessionComplete.Broadcast(false);
        OnSessionError.Broadcast(TEXT("Failed to Create Session (Check Steam)"));
    }
}

void UOutbreakSessionSubsystem::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogTemp, Error, TEXT("[Host] Failed to create Steam Session."));
        OnCreateSessionComplete.Broadcast(false);
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("[Session] Success! Opening Lobby Level..."));
    bIsLobbyHost = true;
    if (bLastRequestDedicated)
    {
        UE_LOG(LogTemp, Log, TEXT("[Host] Dedicated Mode: Requesting AWS GameLift Server..."));
        RequestGameSession(); 
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Host] Listen Mode: Opening L_Lobby with ?listen"));
        UGameplayStatics::OpenLevel(GetWorld(), "L_Lobby", true, "listen");
        OnCreateSessionComplete.Broadcast(true);
    }
}

void UOutbreakSessionSubsystem::FindSessions(int32 MaxResults, FString RoomCode)
{
    if (!IsValidSessionInterface())
    {
        OnSessionError.Broadcast(TEXT("Invalid Session Interface"));
        return;
    }
    
    DesiredRoomCode = RoomCode;
    FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &UOutbreakSessionSubsystem::OnFindSessionsCompleted));
    
    bool IsLAN = IsLanEnvironment();

    LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
    LastSessionSearch->MaxSearchResults = MaxResults;
    LastSessionSearch->bIsLanQuery = IsLAN;
    LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); 
    
    UE_LOG(LogTemp, Log, TEXT("[Session] Searching for sessions..."));
    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
        OnFindSessionsComplete.Broadcast(TArray<FBlueprintSessionResult>(), false);
    }
}

void UOutbreakSessionSubsystem::OnFindSessionsCompleted(bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
    }

    TArray<FBlueprintSessionResult> Results;

    if (bWasSuccessful && LastSessionSearch.IsValid())
    {
        for (const FOnlineSessionSearchResult& Result : LastSessionSearch->SearchResults)
        {
            if (!Result.IsValid()) continue;

            FString FoundGameID;
            FString FoundRoomCode;
            Result.Session.SessionSettings.Get(KEY_GAME_ID, FoundGameID);

            if (FoundGameID != GAME_ID_VALUE) continue;
            if (!DesiredRoomCode.IsEmpty())
            {
                Result.Session.SessionSettings.Get(KEY_ROOM_CODE, FoundRoomCode);
                if (FoundRoomCode != DesiredRoomCode) continue;
            }
            FBlueprintSessionResult BPResult;
            BPResult.OnlineResult = Result;
            Results.Add(BPResult);
        }
        UE_LOG(LogTemp, Log, TEXT("[OnFindSessionsCompleted] Search Finished. Matches Found: %d"), Results.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[OnFindSessionsCompleted] No sessions found or Failed."));
    }

    OnFindSessionsComplete.Broadcast(Results,bWasSuccessful);
}

void UOutbreakSessionSubsystem::JoinSession(const FBlueprintSessionResult& SessionResult)
{
    if (!SessionInterface.IsValid())
    {
        OnSessionError.Broadcast(TEXT("[JoinSession] Invalid Session Interface"));
        return;
    }
    
    JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
    FOnJoinSessionCompleteDelegate::CreateUObject(this, &UOutbreakSessionSubsystem::OnJoinSessionCompleted));
    UE_LOG(LogTemp, Log, TEXT("[JoinSession] Joining Session..."));
    
    FOnlineSessionSearchResult SearchResult = SessionResult.OnlineResult;
    SearchResult.Session.SessionSettings.bUsesPresence = true;
    SearchResult.Session.SessionSettings.bUseLobbiesIfAvailable = true;

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult))
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
        OnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
    }
}

void UOutbreakSessionSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        UE_LOG(LogTemp, Error, TEXT("[Join] Failed to Join Steam Lobby."));
        OnSessionError.Broadcast(TEXT("Failed to Join Steam Lobby"));
        return;
    }

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem)
    {
        OnSessionError.Broadcast(TEXT("[OnJoinSessionCompleted] Invalid Session Interface"));
        return;
    }
    if (!SessionInterface.IsValid())
    {
        OnSessionError.Broadcast(TEXT("[OnJoinSessionCompleted] Invalid Session Interface"));
        return;
    }
    FNamedOnlineSession* Session = SessionInterface->GetNamedSession(SessionName);
    if (!Session)
    {
        OnSessionError.Broadcast(TEXT("[OnJoinSessionCompleted] Session Not Found"));
        return;
    }
    
    FString ServerIP;
    FString GameSessionId;
    int32 ServerPort = 0;

    // Judge Dedicated or Listen Server
    bool bHasDedicatedInfo = 
            Session->SessionSettings.Get(FName("GameSessionId"), GameSessionId) &&
            Session->SessionSettings.Get(FName("SERVER_IP"), ServerIP) &&
            Session->SessionSettings.Get(FName("SERVER_PORT"), ServerPort);
    
    if (bHasDedicatedInfo && !GameSessionId.IsEmpty() && !ServerIP.IsEmpty()) 
    {
        UE_LOG(LogTemp, Warning, TEXT("[OnJoinSessionCompleted] AWS Dedicated Server Found! IP: %s:%d, SessionID: %s"), *ServerIP, ServerPort, *GameSessionId);
        UE_LOG(LogTemp, Log, TEXT("[OnJoinSessionCompleted] Requesting Ticket from Lambda..."));

        RequestJoinTicket(GameSessionId, ServerIP, ServerPort);
    }
    else
    {
        // Listen Server Logic
        UE_LOG(LogTemp, Warning, TEXT("[OnJoinSessionCompleted] No AWS Info found. Attempting P2P Connection..."));

        FString ConnectInfo;
        if (SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
        {
            APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
            if (PC)
            {
                PC->ClientTravel(ConnectInfo, TRAVEL_Absolute);
            }
        }
    }}

void UOutbreakSessionSubsystem::RequestGameSession()
{
    if (!IsSessionOwner()) 
    {
       UE_LOG(LogTemp, Warning, TEXT("[RequestGameSession] Only Host can request GameSession."));
        OnSessionError.Broadcast(TEXT("[RequestGameSession] Only Host can request GameSession."));
       return;
    }
    
    TArray<FString> LobbyMembers = GetSteamLobbyMembers();
    if (LobbyMembers.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[RequestGameSession] No Lobby Members Found (Steam Error?)"));
        OnSessionError.Broadcast(TEXT("Steam Error: Lobby Members Not Found"));
        return;
    }
    UOutbreakAuthSubsystem* AuthSys = GetGameInstance()->GetSubsystem<UOutbreakAuthSubsystem>();
    if (!AuthSys)
    {
        UE_LOG(LogTemp, Error, TEXT("[RequestGameSession] AuthSystem Error"));
        OnSessionError.Broadcast(TEXT("[RequestGameSession] AuthSystem Error"));
        return;
    }
    // Get Steam Ticket from SteamAuth
    FString ticket = AuthSys->GetSteamAuthTicket();
    if (ticket.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("[RequestGameSession] Steam Ticket is not found. Check Out Steam Client! "));
        OnSessionError.Broadcast(TEXT("[RequestGameSession] Steam Ticket is not found. Check Out Steam Client! "));
        return;
    }
    // Get URL from GConfig
    FString RequestURL = GetAwsLambdaUrl();
    if (RequestURL.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("[RequestGameSession] Lambda URL is missing in Game.ini"));
        OnSessionError.Broadcast(TEXT("Config Error: Missing Lambda URL"));
        return;
    }
    
    // Create Json Body
    TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
    RequestObj->SetStringField("ticket", ticket);
    RequestObj->SetStringField("SteamId", AuthSys->GetSteamId());
    TArray<TSharedPtr<FJsonValue>> JsonMembers;
    for (const FString& MemberID : LobbyMembers)
    {
       JsonMembers.Add(MakeShareable(new FJsonValueString(MemberID)));
    }
    RequestObj->SetArrayField("player_ids", JsonMembers);
    // Serialize Json
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer);

    // Create Http Request
    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();
    
    // Lambda URL
    HttpRequest->SetURL(RequestURL);
    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader("Content-Type", "application/json");
    HttpRequest->SetContentAsString(RequestBody);
    HttpRequest->SetTimeout(15.0f);

    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UOutbreakSessionSubsystem::OnGameSessionResponseReceived);
    UE_LOG(LogTemp, Log, TEXT("[RequestGameSession] Requesting GameSession..."));
    HttpRequest->ProcessRequest();
}

void UOutbreakSessionSubsystem::RequestJoinTicket(FString SessionId, FString IP, int32 Port)
{
    SavedJoinIP = IP;
    SavedJoinPort = Port;
    UOutbreakAuthSubsystem* AuthSys = GetGameInstance()->GetSubsystem<UOutbreakAuthSubsystem>();
    FString MySteamId = AuthSys->GetSteamId();

    // Get URL from GConfig
    FString RequestURL = GetAwsLambdaUrl();
    if (RequestURL.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("[RequestGameSession] Lambda URL is missing in Game.ini"));
        OnSessionError.Broadcast(TEXT("Config Error: Missing Lambda URL"));
        return;
    }

    // Create Json Body
    TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
    RequestObj->SetStringField("type", "join");
    RequestObj->SetStringField("SteamId", MySteamId); 
    RequestObj->SetStringField("GameSessionId", SessionId); 

    // Serialize Json
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer);

    // Create Http Request
    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();

    HttpRequest->SetURL(RequestURL);
    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader("Content-Type", "application/json");
    HttpRequest->SetContentAsString(RequestBody);
    HttpRequest->SetTimeout(15.0f); 

    // bind callback 
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UOutbreakSessionSubsystem::OnJoinTicketReceived);
    
    UE_LOG(LogTemp, Log, TEXT("[Join] Sending Ticket Request to AWS..."));
    HttpRequest->ProcessRequest();
}

void UOutbreakSessionSubsystem::OnJoinTicketReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
    bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
    {
        FString ErrorMsg = Response.IsValid() ? FString::Printf(TEXT("HTTP Error: %d"), Response->GetResponseCode()) : TEXT("Connection Timeout");
        UE_LOG(LogTemp, Error, TEXT("[Join] Ticket Request Failed: %s"), *ErrorMsg);
        OnSessionError.Broadcast(TEXT("Join Failed: Server Connection Error"));
        return;
    }
    FString ResponseBody = Response->GetContentAsString();
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        if (JsonObject->HasField("errorMessage"))
        {
            FString ServerError = JsonObject->GetStringField("errorMessage");
            UE_LOG(LogTemp, Error, TEXT("[Join] Lambda Error: %s"), *ServerError);
            OnSessionError.Broadcast(ServerError);
            return;
        }

        if (JsonObject->HasField("ticket"))
        {
            FString Ticket = JsonObject->GetStringField("ticket");
            UE_LOG(LogTemp, Log, TEXT("[Join] Ticket Received! : %s"), *Ticket);

            FString ConnectURL = FString::Printf(TEXT("%s:%d?PlayerSessionId=%s"), *SavedJoinIP, SavedJoinPort, *Ticket);
            
            APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
            if (PC)
            {
                UE_LOG(LogTemp, Warning, TEXT("[Join] Traveling to Dedicated Server..."));
                PC->ClientTravel(ConnectURL, TRAVEL_Absolute);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Join] JSON Response has no 'ticket' field"));
            OnSessionError.Broadcast(TEXT("Invalid Server Response"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Join] JSON Parsing Failed"));
        OnSessionError.Broadcast(TEXT("Data Parsing Error"));
    }
}

FString UOutbreakSessionSubsystem::GetCurrentRoomCode()
{
    if (SessionInterface.IsValid())
    {
        FOnlineSessionSettings* Settings = SessionInterface->GetSessionSettings(NAME_GameSession);
        if (Settings)
        {
            FString Code;
            if (Settings->Get(FName("ROOM_CODE"), Code))
            {
                return Code;
            }
        }
    }
    return FString("UNKNOWN");
}

void UOutbreakSessionSubsystem::OnGameSessionResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[AWS] Request Failed or No Response"));
        OnSessionError.Broadcast(TEXT("Network Connection Failed"));
        return;
    }
    int32 ResponseCode = Response->GetResponseCode();
    switch (ResponseCode)
    {
    case 200:
        {
            UE_LOG(LogTemp,Warning,TEXT("[AWS] Request Successfully Sent"));
        }
        break;
    case 400:
    case 403:
        {
            UE_LOG(LogTemp, Warning, TEXT("[AWS] Client Error: %d"), ResponseCode);
            OnSessionError.Broadcast(TEXT("Invalid Ticket or Request"));
        }
        break;
    case 500:
    case 502:
    case 503:
        {
            UE_LOG(LogTemp, Error, TEXT("[AWS] Server Error: %d"), ResponseCode);
            OnSessionError.Broadcast(TEXT("Server is currently unavailable"));
        }
        break;
    default:
        {
            UE_LOG(LogTemp, Error, TEXT("[AWS] Unknown Code: %d"), ResponseCode);
            OnSessionError.Broadcast(TEXT("Unknown Code"));
        }
        break;
    }
    
    // Deserialize Response Code (IP,Port,Ticket)
    FString ResponseStr = Response->GetContentAsString();
    
    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(ResponseStr);
    
    if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject.IsValid())
    {
       TSharedPtr<FJsonObject> DataObject = RootObject;

       if (RootObject->HasField("body") && RootObject->HasTypedField<EJson::String>("body"))
       {
           FString BodyString = RootObject->GetStringField("body");
           TSharedRef<TJsonReader<TCHAR>> BodyReader = TJsonReaderFactory<TCHAR>::Create(BodyString);
           TSharedPtr<FJsonObject> BodyObject;
           
           if (FJsonSerializer::Deserialize(BodyReader, BodyObject) && BodyObject.IsValid())
           {
               DataObject = BodyObject;
           }
       }
        
       if (DataObject->HasField("errorMessage")) 
       {
           UE_LOG(LogTemp, Error, TEXT("[AWS] Lambda Error: %s"), *DataObject->GetStringField("errorMessage"));
           OnSessionError.Broadcast(DataObject->GetStringField("errorMessage"));
           return;
       }
       
        FString ServerIP = DataObject->GetStringField(TEXT("ip"));
        int32 ServerPort = DataObject->GetNumberField(TEXT("port"));
        FString MyTicket = "";
        FString SessionId = DataObject->GetStringField(TEXT("gamesessionId"));
        
        const TSharedPtr<FJsonObject>* TicketsJson;
        UE_LOG(LogTemp, Log, TEXT("[AWS] Parsed Info - IP: %s, Port: %d"), *ServerIP, ServerPort);

        if (DataObject->TryGetObjectField(TEXT("tickets"), TicketsJson))
        {
            for (auto CurrJsonValue = (*TicketsJson)->Values.CreateConstIterator(); CurrJsonValue; ++CurrJsonValue)
            {
                MyTicket = CurrJsonValue->Value->AsString();
                break; 
            }
        }
        // Travel to Server URL
       if (!ServerIP.IsEmpty() && ServerPort > 0)
       {
          UpdateSteamLobby(ServerIP, ServerPort, MyTicket,SessionId);
           FString ConnectURL;
           if (!MyTicket.IsEmpty())
           {
               ConnectURL = FString::Printf(TEXT("%s:%d?PlayerSessionId=%s"), *ServerIP, ServerPort, *MyTicket);
           }
           else
           {
               ConnectURL = FString::Printf(TEXT("%s:%d"), *ServerIP, ServerPort);
           }
           UE_LOG(LogTemp, Warning, TEXT("[AWS] Travelling to Dedicated Server: %s"), *ConnectURL);
           if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
           {
               PC->ClientTravel(ConnectURL, TRAVEL_Absolute);
           }
       }
       else
       {
           UE_LOG(LogTemp, Error, TEXT("[AWS] Invalid IP or Port received!"));
           OnSessionError.Broadcast(TEXT("Invalid IP or Port"));
       }
    }
}
void UOutbreakSessionSubsystem::UpdateSteamLobby(FString IP, int32 Port, const FString& HostTicket, const FString& SessionId)
{
    if (!SessionInterface.IsValid()) return;
    FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
    if (!ExistingSession) 
    {
        UE_LOG(LogTemp, Warning, TEXT("[UpdateSteamLobby]: ExistingSession Not Found!"));
        return;
    }

    
    FOnlineSessionSettings* SessionSettings = SessionInterface->GetSessionSettings(NAME_GameSession);
    if (!SessionSettings) return;
    if (!IP.IsEmpty() && Port > 0)
    {
        SessionSettings->Set(FName("SERVER_IP"), IP, EOnlineDataAdvertisementType::ViaOnlineService);
        SessionSettings->Set(FName("SERVER_PORT"), Port, EOnlineDataAdvertisementType::ViaOnlineService);
        SessionSettings->Set(FName("GameSessionId"), SessionId, EOnlineDataAdvertisementType::ViaOnlineService);
        SessionSettings->Set(FName("IsDedicated"), true, EOnlineDataAdvertisementType::ViaOnlineService);
        UE_LOG(LogTemp, Log, TEXT("[Steam] Lobby Updated with AWS IP: %s:%d"), *IP, Port);
    }
    else
    {
        SessionSettings->Remove(FName("ServerIP"));
        SessionSettings->Remove(FName("ServerPort"));
    }
    SessionInterface->UpdateSession(NAME_GameSession, *SessionSettings);
}

void UOutbreakSessionSubsystem::OnSteamLobbyUpdated(FName SessionName, const FOnlineSessionSettings& UpdatedSettings)
{
    /*
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
    */
    UE_LOG(LogTemp, Log, TEXT("[OnSteamLobbyUpdated] Steam Lobby Settings Updated via Steam  Network."));
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
    if (bIsLobbyHost) return true;
    if (SessionInterface.IsValid())
    {
        FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession);
        if (Session && Session->bHosting) return true;
    }
    return false;
}

void UOutbreakSessionSubsystem::OnSessionParticipantJoined(FName SessionName, const FUniqueNetId& UniqueId)
{
    if (SessionName != NAME_GameSession) return;
    UE_LOG(LogTemp, Log, TEXT("[OnSessionParticipantJoined] Player Joined: %s"), *UniqueId.ToString());
    
    RefreshLobbyList(); 
}

FString UOutbreakSessionSubsystem::GetAwsLambdaUrl() const
{
    FString LambdaURL;
    if (GConfig)
    {
        GConfig->GetString(
            TEXT("AWS"), 
            TEXT("LambdaURL"), 
            LambdaURL, 
            GGameIni
        );
    }
    LambdaURL = LambdaURL.TrimStartAndEnd();
    if (!LambdaURL.IsEmpty() && !LambdaURL.StartsWith(TEXT("https://")))
    {
        LambdaURL = TEXT("https://") + LambdaURL;
    }
    UE_LOG(LogTemp, Warning, TEXT(" [Config Check] Loaded URL: [%s]"), *LambdaURL);
    return LambdaURL;
}

void UOutbreakSessionSubsystem::OnSessionParticipantLeft(FName SessionName, const FUniqueNetId& UniqueId,
                                                         EOnSessionParticipantLeftReason Reason)
{
    if (SessionName != NAME_GameSession) return;
    UE_LOG(LogTemp, Log, TEXT("[OnSessionParticipantLeft] Player Left: %s (Reason: %d)"), *UniqueId.ToString(), (int32)Reason);
    RefreshLobbyList(); 
}

void UOutbreakSessionSubsystem::RefreshLobbyList()
{
    TArray<FString> CurrentMembers = GetSteamLobbyMembers();
    OnLobbyMembersUpdated.Broadcast(CurrentMembers);
}

