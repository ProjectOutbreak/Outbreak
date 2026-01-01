// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/LobbyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Outbreak/Game/Framework/LobbyGameMode.h"
#include "Subsystems/SessionSubsystem.h"
#include "Utilities/DebugHelper.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindSessionSubsystemCallbacks();
	
	if (Button_GameStart)
	{
		Button_GameStart->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickGameStartButton);
	}
	
	if (SessionsSubsystem && !SessionsSubsystem->IsSessionHost() && Button_GameStart)
	{
		Button_GameStart->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if (SessionsSubsystem)
	{
		FString CurrentCode;
		if (SessionsSubsystem->TryGetCurrentLobbyCode(CurrentCode))
		{
			if (Text_LobbyCode)
			{
				Text_LobbyCode->SetText(FText::FromString(CurrentCode));
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
	SessionsSubsystem = GI->GetSubsystem<USessionSubsystem>();
	if (SessionsSubsystem)
	{
		SessionsSubsystem->OnSessionDestroyComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		SessionsSubsystem->OnSessionError.AddDynamic(this, &ThisClass::OnSessionError);
		SessionsSubsystem->OnSessionStart.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

void ULobbyWidget::RemoveSessionSubsystemCallbacks()
{
	if (SessionsSubsystem)
	{
		SessionsSubsystem->OnSessionDestroyComplete.RemoveDynamic(this, &ThisClass::OnDestroySession);
		SessionsSubsystem->OnSessionError.RemoveDynamic(this, &ThisClass::OnSessionError);
		SessionsSubsystem->OnSessionStart.RemoveDynamic(this, &ThisClass::OnStartSession);
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
	if (!SessionsSubsystem->IsSessionHost())
	{
		PRINT_WITH_CURRENT_CONTEXT("Only the Host can start the game.");
		return;
	}
	
	ALobbyGameMode* GM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;
	
	GM->StartGame();
}
