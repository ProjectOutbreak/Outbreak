// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerListChangedSignature);

UCLASS()
class OUTBREAK_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void UpdatePlayerList();
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerList)
	TArray<FString> PlayerList;
	
	UFUNCTION()
	void OnRep_PlayerList();
	
public:
	// ~ Begin Getters & Setters
	FORCEINLINE const TArray<FString>& GetPlayerList() const { return PlayerList; }
	// ~ End Getters & Setters
};
