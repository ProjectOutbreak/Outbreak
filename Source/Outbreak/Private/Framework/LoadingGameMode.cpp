// Fill out your copyright notice in the Description page of Project Settings.

#include "Outbreak/Public/Framework/LoadingGameMode.h"
#include "Outbreak/Public/Utilities/DebugHelper.h"

void ALoadingGameMode::PlayerIsReady(APlayerController* PC)
{
	NumberOfReadyPlayers++;

	const int32 TargetPlayerNum = 2;
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