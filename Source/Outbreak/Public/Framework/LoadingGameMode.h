// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LoadingGameMode.generated.h"

UCLASS()
class OUTBREAK_API ALoadingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALoadingGameMode();
	void PlayerIsReady(const APlayerController* PC);
	int32 GetTargetPlayerCountFromSession() const;

private:
	int32 NumberOfReadyPlayers = 0;
};
