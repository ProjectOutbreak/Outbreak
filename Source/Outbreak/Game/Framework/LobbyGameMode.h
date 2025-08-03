// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"


UCLASS()
class OUTBREAK_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()


public:
	virtual void BeginPlay() override;
	ALobbyGameMode();

    UPROPERTY(BlueprintReadOnly)
	int32 ConnectedPlayers;

	void PostLogin(APlayerController* NewPlayer) override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	void StartMatchIfReady();	
};
