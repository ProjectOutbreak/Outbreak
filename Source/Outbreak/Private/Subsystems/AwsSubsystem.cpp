// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/AwsSubsystem.h"
#include "EasySessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

void UAwsSubsystem::Deinitialize()
{
	SteamIdentityInterface = nullptr;
	
	Super::Deinitialize();
}

FString UAwsSubsystem::GetSteamId() const
{
	if (IsRunningDedicatedServer())
	{
		UE_LOG(LogTemp, Error, TEXT("Dedicated Server Skipping SteamAuth"))
		return TEXT("Server_No_SteamID");
	}
	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(), SteamName))
	{
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
		if (Identity.IsValid() && Identity->GetUniquePlayerId(0).IsValid())
		{
			return Identity->GetUniquePlayerId(0)->ToString();
		}
	}
	return TEXT("");
}

FString UAwsSubsystem::GetSteamAuthTicket()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(), SteamName);
	if (Subsystem && Subsystem->GetIdentityInterface().IsValid())
	{
		return Subsystem->GetIdentityInterface()->GetAuthToken(0);
	}
	return TEXT("");
}


void UAwsSubsystem::RequestGameSession()
{
	UEasySessionSubsystem* SessionSub = GetGameInstance()->GetSubsystem<UEasySessionSubsystem>();
    if (!SessionSub->IsAdmin()) 
    {
       UE_LOG(LogTemp, Warning, TEXT("[RequestGameSession] Only Host can request GameSession."));
       return;
    }
    
    TArray<FString> LobbyMembers = GetSteamLobbyMembers();
    if (LobbyMembers.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[RequestGameSession] No Lobby Members Found (Steam Error?)"));
        return;
    }
    // Get Steam Ticket from SteamAuth
    FString ticket = GetSteamAuthTicket();
    if (ticket.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("[RequestGameSession] Steam Ticket is not found. Check Out Steam Client! "));
        return;
    }
    // Get URL from GConfig
    FString RequestURL = GetAwsLambdaUrl();
    if (RequestURL.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("[RequestGameSession] Lambda URL is missing in Game.ini"));
        return;
    }
    
    // Create Json Body
    TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
    RequestObj->SetStringField("ticket", ticket);
    RequestObj->SetStringField("SteamId", GetSteamId());
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

    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAwsSubsystem::OnGameSessionResponseReceived);
    UE_LOG(LogTemp, Log, TEXT("[RequestGameSession] Requesting GameSession..."));
    HttpRequest->ProcessRequest();
}

void UAwsSubsystem::RequestJoinTicket(FString SessionId, FString IP, int32 Port)
{
    SavedJoinIP = IP;
    SavedJoinPort = Port;
    UAwsSubsystem* AuthSys = GetGameInstance()->GetSubsystem<UAwsSubsystem>();
    FString MySteamId = AuthSys->GetSteamId();

    // Get URL from GConfig
    FString RequestURL = GetAwsLambdaUrl();
    if (RequestURL.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("[RequestGameSession] Lambda URL is missing in Game.ini"));
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
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAwsSubsystem::OnJoinTicketReceived);
    
    UE_LOG(LogTemp, Log, TEXT("[Join] Sending Ticket Request to AWS..."));
    HttpRequest->ProcessRequest();
}

TArray<FString> UAwsSubsystem::GetSteamLobbyMembers()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld(),TEXT("STEAM"));
	const auto Sessions = Subsystem ? Subsystem->GetSessionInterface() : nullptr;
	TArray<FString> LobbyMembers;
	
	if (Sessions.IsValid())
	{
		if (FNamedOnlineSession* Session = Sessions->GetNamedSession(NAME_GameSession))
		{
			for (const auto& PlayerID : Session->RegisteredPlayers)
			{
				if (PlayerID->IsValid()) LobbyMembers.Add(PlayerID->ToString());
			}
			if (LobbyMembers.Num() == 0)
			{
				if (Subsystem->GetIdentityInterface())
				{
					const auto Id = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
					if (Id.IsValid()) LobbyMembers.Add(Id->ToString());
				}
			}
		}
	}
	
	return LobbyMembers;
}

FString UAwsSubsystem::GetAwsLambdaUrl() const
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

void UAwsSubsystem::OnJoinTicketReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
	{
		FString ErrorMsg = Response.IsValid() ? FString::Printf(TEXT("HTTP Error: %d"), Response->GetResponseCode()) : TEXT("Connection Timeout");
		UE_LOG(LogTemp, Error, TEXT("[Join] Ticket Request Failed: %s"), *ErrorMsg);
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
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Join] JSON Parsing Failed"));
	}
}

void UAwsSubsystem::OnGameSessionResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
	
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[AWS] Request Failed or No Response"));
        return;
    }
    const int32 ResponseCode = Response->GetResponseCode();
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
        }
        break;
    case 500:
    case 502:
    case 503:
        {
            UE_LOG(LogTemp, Error, TEXT("[AWS] Server Error: %d"), ResponseCode);
        }
        break;
    default:
        {
            UE_LOG(LogTemp, Error, TEXT("[AWS] Unknown Code: %d"), ResponseCode);
        }
        break;
    }
    
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
       }
    }
}

void UAwsSubsystem::UpdateSteamLobby(FString IP, int32 Port, const FString& HostTicket, const FString& SessionId)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const auto Sessions = Subsystem ? Subsystem->GetSessionInterface() : nullptr;
	TArray<FString> LobbyMembers;
	
	if (Sessions.IsValid())
	{
		if (Sessions->GetNamedSession(NAME_GameSession)) 
		{
			UE_LOG(LogTemp, Warning, TEXT("[UpdateSteamLobby]: ExistingSession Not Found!"));
			return;
		}

	    
		FOnlineSessionSettings* SessionSettings = Sessions->GetSessionSettings(NAME_GameSession);
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
		Sessions->UpdateSession(NAME_GameSession, *SessionSettings);
	}
}