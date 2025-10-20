// Fill out your copyright notice in the Description page of Project Settings.

#include "Outbreak/Public/Framework/LoadingPlayerController.h"
#include "Outbreak/Public/Framework/LoadingGameMode.h"

void ALoadingPlayerController::Server_NotifyLoadingComplete_Implementation()
{
	if (ALoadingGameMode* LoadingGm = Cast<ALoadingGameMode>(GetWorld()->GetAuthGameMode()))
	{
		LoadingGm->PlayerIsReady(this);
	}
}