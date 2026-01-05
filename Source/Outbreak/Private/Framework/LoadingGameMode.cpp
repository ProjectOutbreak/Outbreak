// Fill out your copyright notice in the Description page of Project Settings.

#include "Outbreak/Public/Framework/LoadingGameMode.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Outbreak/Public/Utilities/DebugHelper.h"

ALoadingGameMode::ALoadingGameMode()
{
	bUseSeamlessTravel = true;
}

void ALoadingGameMode::PlayerIsReady(const APlayerController* PC)
{
	NumberOfReadyPlayers++;

	const int32 TargetPlayerNum = GetTargetPlayerCountFromSession();
	const FString DebugMsg = FString::Printf(TEXT("Player %s is ready. Total ready: %d / %d"), *PC->GetName(), NumberOfReadyPlayers, TargetPlayerNum);
	PRINT_WITH_CURRENT_CONTEXT(DebugMsg);

	if (NumberOfReadyPlayers >= TargetPlayerNum)
	{
		PRINT_WITH_CURRENT_CONTEXT("All players are ready! Traveling to the InGame level...");
		
		const FString MapName = TEXT("/Game/Maps/JM_TestBed");
		const FString InGameModePath = TEXT("/Game/Blueprints/BP_InGameMode.BP_InGameMode_C");
		
		const FString URL = FString::Printf(TEXT("%s?listen?game=%s"), *MapName, *InGameModePath);
		
		GetWorld()->ServerTravel(URL, true);
	}
}

int32 ALoadingGameMode::GetTargetPlayerCountFromSession() const
{
	if (const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession);
			if (Session)
			{
				return Session->RegisteredPlayers.Num();
			}
		}
	}
	
	return 1;
}
