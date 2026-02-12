// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/AwsSubsystem.h"
#include "EasySessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Utilities/DebugHelper.h"

void UAwsSubsystem::Deinitialize()
{
	SteamIdentityInterface = nullptr;
	
	Super::Deinitialize();
}

FString UAwsSubsystem::GetSteamId() const
{
	if (IsRunningDedicatedServer())
	{
		PRINT_WITH_CURRENT_CONTEXT(TEXT("Dedicated Server Skipping SteamAuth"));
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
       PRINT_WITH_CURRENT_CONTEXT(TEXT("Only Host can request GameSession."));
       return;
    }
    
    TArray<FString> LobbyMembers = GetSteamLobbyMembers();
    if (LobbyMembers.Num() == 0)
    {
        FString ErrorMsg = TEXT(" No Lobby Members Found (Steam Error)");
    	OnAwsRequestFailure.Broadcast(ErrorMsg);
        return;
    }
    // Get Steam Ticket from SteamAuth
    FString ticket = GetSteamAuthTicket();
    if (ticket.IsEmpty())
    {
    	FString ErrorMsg = TEXT(" No Ticket Found (Steam Error)");
    	OnAwsRequestFailure.Broadcast(ErrorMsg);
        return;
    }
    // Get URL from GConfig
    FString RequestURL = GetAwsLambdaUrl();
    if (RequestURL.IsEmpty())
    {
    	FString ErrorMsg = TEXT(" No Lambda URL Found (Steam Error)");
    	OnAwsRequestFailure.Broadcast(ErrorMsg);
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
    PRINT_WITH_CURRENT_CONTEXT(TEXT("[RequestGameSession] Requesting GameSession..."));
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
    	FString ErrorMsg = TEXT("No Lambda URL Found (Steam Error)");
    	OnAwsRequestFailure.Broadcast(ErrorMsg);
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
    
    PRINT_WITH_CURRENT_CONTEXT(TEXT("Sending Ticket Request to AWS..."));
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
	PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Loaded URL: [%s]"), *LambdaURL));
	return LambdaURL;
}

void UAwsSubsystem::OnJoinTicketReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
	{
		FString ErrorMsg = Response.IsValid() ? FString::Printf(TEXT("HTTP Error: %d"), Response->GetResponseCode()) : TEXT("Connection Timeout");
		OnAwsRequestFailure.Broadcast(ErrorMsg);
		return;
	}
	const int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		OnAwsRequestFailure.Broadcast(FString::Printf(TEXT("Ticket Server Error: %d"), ResponseCode));
		return;
	}
	
	FString ResponseBody = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		OnAwsRequestFailure.Broadcast(TEXT("Invalid Ticket Data Format"));
		return;
	}
	if (JsonObject->HasField("errorMessage"))
	{
		FString ServerError = JsonObject->GetStringField("errorMessage");
		OnAwsRequestFailure.Broadcast(TEXT("Invalid Ticket Data Format"));
		return;
	}
	if (!JsonObject->HasField("ticket"))
	{
		OnAwsRequestFailure.Broadcast(TEXT("Ticket Not Found"));
	}
	FString Ticket = JsonObject->GetStringField("ticket");
	if (Ticket.IsEmpty())
	{
		OnAwsRequestFailure.Broadcast(TEXT("Ticket Not Found in Response"));
		return;
	}
	FString ConnectURL = FString::Printf(TEXT("%s:%d?PlayerSessionId=%s"), *SavedJoinIP, SavedJoinPort, *Ticket);
            
	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
	if (PC)
	{
		PRINT_WITH_CURRENT_CONTEXT(TEXT("Traveling to Dedicated Server"));
		PC->ClientTravel(ConnectURL, TRAVEL_Absolute);
	}
}

void UAwsSubsystem::OnGameSessionResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
	
    if (!bWasSuccessful || !Response.IsValid())
    {
    	OnAwsRequestFailure.Broadcast(TEXT("Connection Failed")); 
    	return;        
    }
	
    const int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		OnAwsRequestFailure.Broadcast(FString::Printf(TEXT("Server Error: %d"), ResponseCode));
		return;
	}
	
    FString ResponseStr = Response->GetContentAsString();
    
    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(ResponseStr);
	
	if (!FJsonSerializer::Deserialize(Reader, RootObject))
	{
		OnAwsRequestFailure.Broadcast(TEXT("Invalid Data Format"));
		return;
	}
	
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
		FString ErrorMsg = RootObject->GetStringField("errorMessage");
       	OnAwsRequestFailure.Broadcast(ErrorMsg);
        return;
    }
	
    FString ServerIP = DataObject->GetStringField(TEXT("ip"));
    int32 ServerPort = DataObject->GetNumberField(TEXT("port"));
    FString MyTicket = "";
    FString SessionId = DataObject->GetStringField(TEXT("gamesessionId"));
    
    const TSharedPtr<FJsonObject>* TicketsJson;
    PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("[AWS] Parsed Info - IP: %s, Port: %d"), *ServerIP, ServerPort));
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
		PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("[AWS] Travelling to Dedicated Server: %s"), *ConnectURL));
		if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
		{
		    PC->ClientTravel(ConnectURL, TRAVEL_Absolute);
		}
	}
	else
	{
		PRINT_WITH_CURRENT_CONTEXT(TEXT("[AWS] Invalid IP or Port received!"));
	}	
}


void UAwsSubsystem::UpdateSteamLobby(FString IP, int32 Port, const FString& HostTicket, const FString& SessionId)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	const auto Sessions = Subsystem ? Subsystem->GetSessionInterface() : nullptr;
	TArray<FString> LobbyMembers;
	
	if (Sessions.IsValid())
	{
		if (!Sessions->GetNamedSession(NAME_GameSession)) 
		{
			PRINT_WITH_CURRENT_CONTEXT(TEXT("ExistingSession Not Found!"));
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
			PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Lobby Updated with AWS IP: %s:%d"), *IP, Port));
		}
		else
		{
			SessionSettings->Remove(FName("ServerIP"));
			SessionSettings->Remove(FName("ServerPort"));
		}
		Sessions->UpdateSession(NAME_GameSession, *SessionSettings);
	}
}