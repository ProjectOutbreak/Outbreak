// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/LobbyWidget.h"
#include "EasySessionSubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "OnlineSubsystemUtils.h"
#include "Framework/GameState/LobbyGameState.h"
#include "Utilities/DebugHelper.h"
#include "Game/Controller/LobbyPlayerController.h"

bool ULobbyWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	
	OnPlayerListUpdate();	
	return true;
}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Button_GameStart)
	{
		Button_GameStart->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickGameStartButton);
	}
	
	if (!SessionsSubsystem)
	{
		SessionsSubsystem = GetGameInstance()->GetSubsystem<UEasySessionSubsystem>();
	}
	
	if (SessionsSubsystem->IsAdmin())
	{
		Button_GameStart->SetVisibility(ESlateVisibility::Visible);
		Button_GameStart->SetIsEnabled(true); 
	}
	else
	{
		Button_GameStart->SetVisibility(ESlateVisibility::Collapsed);
	}

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;

	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(NAME_GameSession);

	if (CurrentSession && CurrentSession->SessionSettings.Settings.Num() > 0)
	{
		if (const FOnlineSessionSetting* Setting = CurrentSession->SessionSettings.Settings.Find(FName("ROOM_CODE")))
		{
			if (Text_LobbyCode)
			{
				const FString Prefix = TEXT("입장 코드 : ");
				FString OutCode;
				Setting->Data.GetValue(OutCode);
				Text_LobbyCode->SetText(FText::FromString(Prefix + OutCode));
			}
		}
	}
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
	
	if (ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>())
	{
		GS->OnPlayerListChanged.AddDynamic(this, &ThisClass::OnPlayerListUpdate);
        
		OnPlayerListUpdate();
	}
	
	GetWorld()->GetTimerManager().SetTimer(PlayerListTimerHandle, this, &ThisClass::OnPlayerListUpdate, 1.0f, true);
    
	OnPlayerListUpdate(); 
}

void ULobbyWidget::NativeDestruct()
{
	RemoveFromParent();
	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			const FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
	Super::NativeDestruct();
}

void ULobbyWidget::OnClickGameStartButton()
{
	if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		PRINT_WITH_CURRENT_CONTEXT("Requesting Game Start to Server...");
		PC->RequestStartGame();
        
		Button_GameStart->SetIsEnabled(false); 
	}
}

void ULobbyWidget::OnPlayerListUpdate()
{
	if (!Text_PlayerList) return;

	TArray<FString> PlayerNames;
	
	if (ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>())
	{
		PlayerNames = GS->GetPlayerNames();
		TArray<FString> Names = GS->GetPlayerNames();
	}
	else
	{
		PRINT_WITH_CURRENT_CONTEXT("Error: Owning Player is not ALobbyGameState!");
		return;
	}
	FString FormattedPlayerList = TEXT("Players:\n");
	for (int32 i = 0; i < PlayerNames.Num(); ++i)
	{
		FormattedPlayerList.Append(FString::Printf(TEXT("%d. %s\n"), i + 1, *PlayerNames[i]));
	}
    
	Text_PlayerList->SetText(FText::FromString(FormattedPlayerList));
}