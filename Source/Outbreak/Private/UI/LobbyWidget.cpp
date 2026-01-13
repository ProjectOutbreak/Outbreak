// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/LobbyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Framework/GameState/LobbyGameState.h"
#include "Outbreak/Game/Framework/LobbyGameMode.h"
#include "Game/Framework/OutbreakSessionSubsystem.h"
#include "Utilities/DebugHelper.h"
#include "Game/Controller/LobbyPlayerController.h"
#include "Framework/GameState/LobbyGameState.h"
bool ULobbyWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	
	OnPlayerListUpdate();	
	return true;
}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	BindSessionSubsystemCallbacks();
	
	if (Button_GameStart)
	{
		Button_GameStart->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickGameStartButton);
	}
	if (SessionsSubsystem)
	{
		if (SessionsSubsystem->IsSessionOwner())
		{
			Button_GameStart->SetVisibility(ESlateVisibility::Visible);
			Button_GameStart->SetIsEnabled(true); 
		}
		else
		{
			Button_GameStart->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (Text_LobbyCode)
		{
			Text_LobbyCode->SetText(FText::FromString(SessionsSubsystem->GetCurrentRoomCode()));
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
	RemoveSessionSubsystemCallbacks();
	Super::NativeDestruct();
}

void ULobbyWidget::BindSessionSubsystemCallbacks()
{
	const UGameInstance* GI = GetGameInstance();
	SessionsSubsystem = GI->GetSubsystem<UOutbreakSessionSubsystem>();
	if (SessionsSubsystem)
	{
		SessionsSubsystem->OnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		SessionsSubsystem->OnSessionError.AddDynamic(this, &ThisClass::OnSessionError);
		//SessionsSubsystem->OnSessionStart.AddDynamic(this, &ThisClass::OnStartSession);
		SessionsSubsystem->OnLobbyMembersUpdated.AddDynamic(this, &ThisClass::OnLobbyMembersUpdated);
	}
}

void ULobbyWidget::RemoveSessionSubsystemCallbacks()
{
	if (SessionsSubsystem)
	{
		SessionsSubsystem->OnDestroySessionComplete.RemoveDynamic(this, &ThisClass::OnDestroySession);
		SessionsSubsystem->OnSessionError.RemoveDynamic(this, &ThisClass::OnSessionError);
		//SessionsSubsystem->OnSessionStart.RemoveDynamic(this, &ThisClass::OnStartSession);
		SessionsSubsystem->OnLobbyMembersUpdated.RemoveDynamic(this, &ThisClass::OnLobbyMembersUpdated);
	}
}

void ULobbyWidget::OnDestroySession(bool bWasSuccessful)
{
	const FString DebugMsg = FString::Printf(TEXT("Session Destroyed: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failure"));
	PRINT_WITH_CURRENT_CONTEXT(DebugMsg);
}

void ULobbyWidget::OnSessionError(const FString& Reason)
{
	PRINT_WITH_CURRENT_CONTEXT(Reason);
}

void ULobbyWidget::OnStartSession(bool bWasSuccessful)
{
	const FString DebugMsg = FString::Printf(TEXT("Session Started: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failure"));
	PRINT_WITH_CURRENT_CONTEXT(DebugMsg);
}

void ULobbyWidget::OnClickGameStartButton()
{
	if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		PRINT_WITH_CURRENT_CONTEXT("Requesting Game Start to Server...");
		PC->RequestStartGame();
        
		Button_GameStart->SetIsEnabled(false); 
	}
	else
	{
		PRINT_WITH_CURRENT_CONTEXT("Error: Owning Player is not ALobbyPlayerController!");
	}}

void ULobbyWidget::OnPlayerListUpdate()
{
	if (!Text_PlayerList) return;

	TArray<FString> PlayerNames;
	
	if (ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>())
	{
		PlayerNames = GS->GetPlayerNames();
		TArray<FString> Names = GS->GetPlayerNames();
    
		//UE_LOG(LogTemp, Warning, TEXT(">> Widget Update! Count: %d <<"), Names.Num());
	}
	else
	{
		PRINT_WITH_CURRENT_CONTEXT("Error: Owning Player is not ALobbyGameState!");
		UE_LOG(LogTemp, Error, TEXT(">> GameState is NULL or NOT LobbyGameState! <<"));
		return;
	}
	FString FormattedPlayerList = TEXT("Players:\n");
	for (int32 i = 0; i < PlayerNames.Num(); ++i)
	{
		FormattedPlayerList.Append(FString::Printf(TEXT("%d. %s\n"), i + 1, *PlayerNames[i]));
	}
    
	Text_PlayerList->SetText(FText::FromString(FormattedPlayerList));}
void ULobbyWidget::OnLobbyMembersUpdated(const TArray<FString>& Members)
{
}	// OnPlayerListUpdate(Members); 
