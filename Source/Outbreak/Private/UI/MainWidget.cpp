// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainWidget.h"
#include "EasySessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Utilities/DebugHelper.h"

const FName KEY_ROOM_CODE(TEXT("ROOM_CODE"));

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
	RemoveSessionSubsystemCallbacks();
	
	Super::NativeDestruct();
}

void UMainWidget::SetButtonsEnabled(bool BNewIsEnabled)
{
	Button_CreateLobby->SetIsEnabled(BNewIsEnabled);
	Button_JoinLobby->SetIsEnabled(BNewIsEnabled);
	Button_SinglePlay->SetIsEnabled(BNewIsEnabled);
}

FString UMainWidget::GenerateRandomLobbyCode(int32 Length)
{
	const FString Chars = TEXT("ABCDEFGHJKLMNPQRSTUVWXYZ23456789");
	FString RandomCode;
	for (int32 i = 0; i < Length; ++i)
	{
		RandomCode += Chars[FMath::RandRange(0, Chars.Len() - 1)];
	}
	
	FPlatformApplicationMisc::ClipboardCopy(*RandomCode);
	
	return RandomCode;
}

void UMainWidget::BindSessionSubsystemCallbacks()
{
	const UGameInstance* GameInstance = GetGameInstance();
	SessionsSubsystem = GameInstance->GetSubsystem<UEasySessionSubsystem>();
	if (SessionsSubsystem)
	{
		SessionsSubsystem->OnStartSessionFailure.AddUObject(this, &ThisClass::OnStartSessionFailure);
		SessionsSubsystem->OnFindSessionsSuccess.AddUObject(this, &ThisClass::OnFindSessionSuccess);
		SessionsSubsystem->OnFindSessionsFailure.AddUObject(this, &ThisClass::OnFindSessionFailure);
		SessionsSubsystem->OnJoinSessionSuccess.AddUObject(this, &ThisClass::OnJoinSessionSuccess);
		SessionsSubsystem->OnJoinSessionFailure.AddUObject(this, &ThisClass::OnJoinSessionFailure);
		SessionsSubsystem->OnDestroySessionSuccess.AddUObject(this, &ThisClass::OnDestroySessionSuccess);
		SessionsSubsystem->OnDestroySessionFailure.AddUObject(this, &ThisClass::OnDestroySessionFailure);
	}
}

void UMainWidget::RemoveSessionSubsystemCallbacks()
{
	if (SessionsSubsystem)
	{
		SessionsSubsystem->OnStartSessionFailure.RemoveAll(this);
		SessionsSubsystem->OnFindSessionsSuccess.RemoveAll(this);
		SessionsSubsystem->OnFindSessionsFailure.RemoveAll(this);
		SessionsSubsystem->OnJoinSessionSuccess.RemoveAll(this);
		SessionsSubsystem->OnJoinSessionFailure.RemoveAll(this);
		SessionsSubsystem->OnDestroySessionSuccess.RemoveAll(this);
		SessionsSubsystem->OnDestroySessionFailure.RemoveAll(this);
	}
}

void UMainWidget::OnStartSessionFailure()
{
	SetButtonsEnabled(false);
}

void UMainWidget::OnFindSessionSuccess(const TArray<FOnlineSessionSearchResult>& SessionResults)
{
	const FString LobbyCode = ETB_LobbyCode->GetText().ToString().ToUpper();

	if (SessionResults.Num() == 0)
	{
		SetButtonsEnabled(true);
		PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Failed to find session or No Results for: %s"), *LobbyCode));
		return;
	}

	PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Found %d Sessions. Checking codes..."), SessionResults.Num()));

	for (const FOnlineSessionSearchResult& Result : SessionResults)
	{
		FString FoundCode;
		Result.Session.SessionSettings.Get(KEY_ROOM_CODE, FoundCode);

		if (FoundCode == LobbyCode)
		{
			PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Match Found! Joining: %s"), *FoundCode));
			SessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	PRINT_WITH_CURRENT_CONTEXT("No matched lobby code among results");
	SetButtonsEnabled(true);
}

void UMainWidget::OnFindSessionFailure(const TArray<FOnlineSessionSearchResult>& SessionResults)
{
	SetButtonsEnabled(true);
}

void UMainWidget::OnJoinSessionSuccess()
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;
	
	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

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
}

void UMainWidget::OnJoinSessionFailure()
{
	SetButtonsEnabled(true);
}

void UMainWidget::OnDestroySessionSuccess()
{
	PRINT_WITH_CURRENT_CONTEXT("Session Destroyed Successfully");
}

void UMainWidget::OnDestroySessionFailure()
{
	PRINT_WITH_CURRENT_CONTEXT("Failed to Destroy Session");
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
		int32 NumPublicConnections = 4;
       
		bool bUseDedicated;
		if (CheckBox_UseDedicated)
		{
			bUseDedicated = CheckBox_UseDedicated->IsChecked();
		}
		else
		{
			bUseDedicated = false; 
		}

		PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Hosting Game... Dedicated: %d"), bUseDedicated));
		
		FEasySessionSettings Settings;
		Settings.NumPublicConnections = NumPublicConnections;
		Settings.bIsDedicated = bUseDedicated;
		Settings.CustomProperties.Add(GKey_Lobby_Code.ToString(), CachedCreatedLobbyCode);

		SessionsSubsystem->CreateSession(Settings);
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
		FEasySearchSettings SearchSettings;
		SearchSettings.QuerySettings.Add(GKey_Lobby_Code.ToString(), ETB_LobbyCode->GetText().ToString().ToUpper());
		SessionsSubsystem->FindSessions(SearchSettings);
	}
}

void UMainWidget::OnClickSinglePlayButton()
{
	// TODO : Single Play 구현
}