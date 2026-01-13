// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainWidget.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Game/Framework/LobbyGameMode.h"
#include "Game/Framework/OutbreakSessionSubsystem.h"
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
	SessionsSubsystem = GameInstance->GetSubsystem<UOutbreakSessionSubsystem>();
	if (SessionsSubsystem)
	{
		SessionsSubsystem->OnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		SessionsSubsystem->OnFindSessionsComplete.AddDynamic(this, &ThisClass::OnFindSession);
		SessionsSubsystem->OnJoinSessionComplete.AddDynamic(this, &ThisClass::OnJoinSession);
		SessionsSubsystem->OnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		SessionsSubsystem->OnSessionError.AddDynamic(this, &ThisClass::OnSessionError);

		//SessionsSubsystem->OnSessionStart.AddDynamic(this, &ThisClass::OnStartSession);
	}
	UE_LOG(LogTemp, Warning, TEXT("[UI] MainWidget Created & Bound!"));
}

void UMainWidget::RemoveSessionSubsystemCallbacks()
{
	if (SessionsSubsystem)
	{
		SessionsSubsystem->OnCreateSessionComplete.RemoveDynamic(this, &ThisClass::OnCreateSession);
		SessionsSubsystem->OnFindSessionsComplete.RemoveDynamic(this, &ThisClass::OnFindSession);
		SessionsSubsystem->OnJoinSessionComplete.RemoveDynamic(this, &ThisClass::OnJoinSession);
		SessionsSubsystem->OnDestroySessionComplete.RemoveDynamic(this, &ThisClass::OnDestroySession);
		SessionsSubsystem->OnSessionError.RemoveDynamic(this, &ThisClass::OnSessionError);
		//SessionsSubsystem->OnSessionStart.RemoveDynamic(this, &ThisClass::OnStartSession);
	}
}

// -------------------------- Callback ------------------------------- // 

void UMainWidget::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINT_WITH_CURRENT_CONTEXT("Session Created Successfully.");
	}     
	else
	{
		PRINT_WITH_CURRENT_CONTEXT("Failed to Create Session");
		SetButtonsEnabled(true);
	}
}

void UMainWidget::OnFindSession(const TArray<FBlueprintSessionResult>& SessionResults, bool bWasSuccessful)
{
	if (SessionsSubsystem == nullptr)
	{
		SetButtonsEnabled(true);
		return;
	}
    
	const FString LobbyCode = ETB_LobbyCode->GetText().ToString().ToUpper();

	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		SetButtonsEnabled(true);
		PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Failed to find session or No Results for: %s"), *LobbyCode));
		return;
	}

	PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Found %d Sessions. Checking codes..."), SessionResults.Num()));

	for (const FBlueprintSessionResult& Result : SessionResults)
	{
		FString FoundCode;
		// BlueprintResult.OnlineResult 로 원본 접근
		Result.OnlineResult.Session.SessionSettings.Get(KEY_ROOM_CODE, FoundCode);

		if (FoundCode == LobbyCode)
		{
			PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Match Found! Joining: %s"), *FoundCode));
			SessionsSubsystem->JoinSession(Result); // Join 호출
			return;
		}
	}

	PRINT_WITH_CURRENT_CONTEXT("No matched lobby code among results");
	SetButtonsEnabled(true);
}
void UMainWidget::OnJoinSession(int32 Result)
{
	EOnJoinSessionCompleteResult::Type ResultEnum = (EOnJoinSessionCompleteResult::Type)Result;
	SetButtonsEnabled(true);

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		FString FailReason = JoinSessionResultToText(ResultEnum);
		PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Join Failed: %s"), *FailReason));
		return;	}

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
	PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Session Destroyed: %d"), bWasSuccessful));
}

void UMainWidget::OnSessionError(const FString& Reason)
{
	PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Session Error: %s"), *Reason));
	SetButtonsEnabled(true);
}

void UMainWidget::OnStartSession(bool bWasSuccessful)
{
	const FString DebugMsg = FString::Printf(TEXT("Session Started: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failure"));
	PRINT_WITH_CURRENT_CONTEXT(DebugMsg);
}

// -------------------------- Button Click Event ------------------------------- // 

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
       
		bool bUseDedicated = false;
		if (CheckBox_UseDedicated)
		{
			bUseDedicated = CheckBox_UseDedicated->IsChecked();
		}
		else
		{
			bUseDedicated = false; 
		}

		PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Hosting Game... Dedicated: %d"), bUseDedicated));
		SessionsSubsystem->HostGame(CachedCreatedLobbyCode, NumPublicConnections, bUseDedicated);
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
		SessionsSubsystem->FindSessions(100, ETB_LobbyCode->GetText().ToString().ToUpper());
	}
}

void UMainWidget::OnClickSinglePlayButton()
{
	// TODO : Single Play 구현
}