// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainWidget.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/SessionSubsystem.h"
#include "Utilities/DebugHelper.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	BindSessionSubsystemCallbacks();

	if (CreateGameButton)	CreateGameButton->OnClicked.AddDynamic(this, &UMainWidget::OnClickCreateGameButton);
	if (JoinGameButton)		JoinGameButton->OnClicked.AddDynamic(this, &UMainWidget::OnClickJoinGameButton);
	if (SinglePlayButton)	SinglePlayButton->OnClicked.AddDynamic(this, &UMainWidget::OnClickSinglePlayButton);
}

void UMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	RemoveSessionSubsystemCallbacks();
}

void UMainWidget::SetButtonsEnabled(bool BNewIsEnabled)
{
	CreateGameButton->SetIsEnabled(BNewIsEnabled);
	JoinGameButton->SetIsEnabled(BNewIsEnabled);
	SinglePlayButton->SetIsEnabled(BNewIsEnabled);
}

const TCHAR* UMainWidget::JoinSessionResultToText(const EOnJoinSessionCompleteResult::Type InResult) const
{
	switch (InResult)
	{
	case EOnJoinSessionCompleteResult::Success:               return TEXT("Success");
	case EOnJoinSessionCompleteResult::SessionIsFull:         return TEXT("SessionIsFull");
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:   return TEXT("SessionDoesNotExist");
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:return TEXT("CouldNotRetrieveAddress");
	case EOnJoinSessionCompleteResult::AlreadyInSession:      return TEXT("AlreadyInSession");
	default:                                                  return TEXT("Unknown");
	}
}

FString UMainWidget::GenerateRandomLobbyCode(int32 Length)
{
	const FString Chars = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	FString RandomCode;
	for (int32 i = 0; i < Length; ++i)
	{
		RandomCode += Chars[FMath::RandRange(0, Chars.Len() - 1)];
	}
	
	FPlatformApplicationMisc::ClipboardCopy(*RandomCode);
	PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Lobby Code copied to Clipboard: %s"), *RandomCode));
	
	return RandomCode;
}

void UMainWidget::BindSessionSubsystemCallbacks()
{
	UGameInstance* GameInstance = GetGameInstance();
	SessionsSubsystem = GameInstance->GetSubsystem<USessionSubsystem>();
	if (SessionsSubsystem)
	{
		SessionsSubsystem->OnSessionCreateComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		SessionsSubsystem->OnSessionSearchFinished.AddUObject(this, &ThisClass::OnFindSession);
		SessionsSubsystem->OnSessionJoinComplete.AddUObject(this, &ThisClass::OnJoinSession);
		SessionsSubsystem->OnSessionDestroyComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		SessionsSubsystem->OnSessionError.AddDynamic(this, &ThisClass::OnSessionError);
		SessionsSubsystem->OnSessionStart.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

void UMainWidget::RemoveSessionSubsystemCallbacks()
{
	if (SessionsSubsystem)
	{
		SessionsSubsystem->OnSessionCreateComplete.RemoveDynamic(this, &ThisClass::OnCreateSession);
		SessionsSubsystem->OnSessionDestroyComplete.RemoveDynamic(this, &ThisClass::OnDestroySession);
		SessionsSubsystem->OnSessionError.RemoveDynamic(this, &ThisClass::OnSessionError);
		SessionsSubsystem->OnSessionStart.RemoveDynamic(this, &ThisClass::OnStartSession);
		SessionsSubsystem->OnSessionSearchFinished.RemoveAll(this);
		SessionsSubsystem->OnSessionJoinComplete.RemoveAll(this);
	}
}

void UMainWidget::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINT_WITH_CURRENT_CONTEXT("Session Created Successfully");
		const FString MapName = "L_Loading";
		const FString Options = "?listen";
	
		UGameplayStatics::OpenLevel(this, FName(* (MapName + Options)));
	}
	else
	{
		PRINT_WITH_CURRENT_CONTEXT("Failed to Create Session");
		SetButtonsEnabled(true);
	}
}

void UMainWidget::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (SessionsSubsystem == nullptr)
	{
		PRINT_WITH_CURRENT_CONTEXT("SessionsSubsystem is nullptr");
		SetButtonsEnabled(true);
		return;
	}
	const FString& LobbyCode = TB_LobbyCode->GetText().ToString();

	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		SetButtonsEnabled(true);
		Debug::Print(FString::Printf(TEXT("Failed to find session with LobbyCode %s on [OnFindSession]"), *LobbyCode));
		return;
	}

	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		FString Code;
		Result.Session.SessionSettings.Get(USessionSubsystem::KEY_LOBBY_CODE, Code);
		const FString Owner = Result.Session.OwningUserName;
		const int32 Ping = Result.PingInMs;

		const FString DebugMsg = FString::Printf(TEXT("Found Session - Code: %s, Owner: %s, Ping: %d"), *Code, *Owner, Ping);
		PRINT_WITH_CURRENT_CONTEXT(DebugMsg);

		Result.Session.SessionSettings.Get(SessionsSubsystem->KEY_LOBBY_CODE, SettingsValue);
		if (SettingsValue == LobbyCode)
		{
			Result.Session.SessionSettings.bUseLobbiesIfAvailable = true;
			Result.Session.SessionSettings.bUsesPresence = true;
			SessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	PRINT_WITH_CURRENT_CONTEXT("No matched lobby code among results");
	SetButtonsEnabled(true);
}

void UMainWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	SetButtonsEnabled(true);

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		const FString Reason = FString::Printf(TEXT("Join failed: %s (LAN=%d, OSS=%s)"), 
			JoinSessionResultToText(Result), 
			(int32)SessionsSubsystem->IsLanEnvironment(),
			Online::GetSubsystem(GetWorld()) ? *Online::GetSubsystem(GetWorld())->GetSubsystemName().ToString() : TEXT("None"));
		
		PRINT_WITH_CURRENT_CONTEXT(Reason);
		return;
	}

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem)
	{
		PRINT_WITH_CURRENT_CONTEXT("No OnlineSubsystem found when trying to join");
		return;
	}
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		PRINT_WITH_CURRENT_CONTEXT("No valid SessionInterface found when trying to join");
		return;
	}

	FString Address;
	SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

	if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
	{
		PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

void UMainWidget::OnDestroySession(bool bWasSuccessful)
{
	const FString DebugMsg = FString::Printf(TEXT("Session Destroyed: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failure"));
	PRINT_WITH_CURRENT_CONTEXT(DebugMsg);
}

void UMainWidget::OnSessionError(const FString& Reason)
{
	PRINT_WITH_CURRENT_CONTEXT(Reason);
}

void UMainWidget::OnStartSession(bool bWasSuccessful)
{
	const FString DebugMsg = FString::Printf(TEXT("Session Started: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failure"));
	PRINT_WITH_CURRENT_CONTEXT(DebugMsg);
}

void UMainWidget::OnClickCreateGameButton()
{
	SetButtonsEnabled(false);
	
	FString LobbyCode;
	if (TB_LobbyCode->GetText().IsEmpty())
	{
		LobbyCode = GenerateRandomLobbyCode(5);
	}
	else
	{
		LobbyCode = TB_LobbyCode->GetText().ToString().ToUpper();
	}
	
	if (SessionsSubsystem)
	{
		int NumPublicConnections = 4;
		SessionsSubsystem->CreateSession(NumPublicConnections, LobbyCode);
	}
}

void UMainWidget::OnClickJoinGameButton()
{
	if (TB_LobbyCode->GetText().IsEmpty())
	{
		PRINT_WITH_CURRENT_CONTEXT("Lobby Code is Empty");
		return;
	}
	SetButtonsEnabled(false);
	if (SessionsSubsystem)
	{
		SessionsSubsystem->FindSessions(10000, TB_LobbyCode->GetText().ToString().ToUpper());
	}
}

void UMainWidget::OnClickSinglePlayButton()
{
	UGameplayStatics::OpenLevel(this, FName("L_Loading"));
}
