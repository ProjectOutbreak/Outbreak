// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainWidget.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Game/Framework/LobbyGameMode.h"
#include "Subsystems/SessionSubsystem.h"
#include "Utilities/DebugHelper.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	BindSessionSubsystemCallbacks();

	if (Button_CreateLobby)	Button_CreateLobby->OnClicked.AddDynamic(this, &UMainWidget::OnClickCreateGameButton);
	if (Button_JoinLobby)		Button_JoinLobby->OnClicked.AddDynamic(this, &UMainWidget::OnClickJoinGameButton);
	if (Button_SinglePlay)	Button_SinglePlay->OnClicked.AddDynamic(this, &UMainWidget::OnClickSinglePlayButton);
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void UMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	RemoveSessionSubsystemCallbacks();
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
}

void UMainWidget::SetButtonsEnabled(bool BNewIsEnabled)
{
	Button_CreateLobby->SetIsEnabled(BNewIsEnabled);
	Button_JoinLobby->SetIsEnabled(BNewIsEnabled);
	Button_SinglePlay->SetIsEnabled(BNewIsEnabled);
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
		PRINT_WITH_CURRENT_CONTEXT("Session Created Successfully. Opening Listen Server...");

		const FString LobbyLevelName = "L_Lobby";
		const FString Option = "?listen";
		UGameplayStatics::OpenLevel(GetWorld(), FName(*LobbyLevelName + Option));
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
	const FString& LobbyCode = ETB_LobbyCode->GetText().ToString();

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
	if (Address.Contains(TEXT(":0")))
	{
		Address.ReplaceInline(TEXT(":0"), TEXT(":7777"));
	}
	else if (!Address.Contains(TEXT(":")))
	{
		Address += TEXT(":7777");
	}
	PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Joining session at address: %s"), *Address));

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
	
	if (ETB_LobbyCode->GetText().IsEmpty())
	{
		CachedCreatedLobbyCode = GenerateRandomLobbyCode(5);
	}
	else
	{
		CachedCreatedLobbyCode = ETB_LobbyCode->GetText().ToString().ToUpper();
	}
	
	if (SessionsSubsystem)
	{
		int NumPublicConnections = 4;
		SessionsSubsystem->CreateSession(NumPublicConnections, CachedCreatedLobbyCode);
	}
}

void UMainWidget::OnClickJoinGameButton()
{
	if (ETB_LobbyCode->GetText().IsEmpty())
	{
		PRINT_WITH_CURRENT_CONTEXT("Lobby Code is Empty");
		return;
	}
	SetButtonsEnabled(false);
	if (SessionsSubsystem)
	{
		SessionsSubsystem->FindSessions(10000, ETB_LobbyCode->GetText().ToString().ToUpper());
	}
}

void UMainWidget::OnClickSinglePlayButton()
{
	// TODO : Single Play 구현
}