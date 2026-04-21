// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerListChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyAdminChanged);

UCLASS()
class OUTBREAK_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdatePlayerList();
	
	FOnPlayerListChangedSignature OnPlayerListChanged;
	FOnLobbyAdminChanged OnLobbyAdminChangedDelegate;

	TArray<FString> GetPlayerNames();

	UPROPERTY(ReplicatedUsing = OnRep_PlayerList)
	TArray<FString> PlayerList;

	UPROPERTY(ReplicatedUsing = OnRep_LobbyAdmin)
	TObjectPtr<APlayerState> LobbyAdmin;
	
	UFUNCTION()
	void OnRep_PlayerList();
	
	UFUNCTION()
	void OnRep_LobbyAdmin();

protected:

	
public:
	// ~ Begin Getters & Setters
	FORCEINLINE const TArray<FString>& GetPlayerList() const { return PlayerList; }
	// ~ End Getters & Setters
};
