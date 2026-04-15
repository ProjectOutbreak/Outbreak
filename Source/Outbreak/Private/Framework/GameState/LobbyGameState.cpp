// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/GameState/LobbyGameState.h"

#include "AI/NavigationSystemBase.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void ALobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if (OnPlayerListChanged.IsBound())
	{
		OnPlayerListChanged.Broadcast();
	}
}

void ALobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	
	//UpdatePlayerList();
	if (OnPlayerListChanged.IsBound())
	{
		OnPlayerListChanged.Broadcast();
	}
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, PlayerList);
	DOREPLIFETIME(ThisClass, LobbyAdmin);
}

void ALobbyGameState::UpdatePlayerList()
{
	if (!HasAuthority()) return;

	TArray<FString> NewPlayerList;
	for (const APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState)
		{
			NewPlayerList.Add(PlayerState->GetPlayerName());
		}
	}

	PlayerList = NewPlayerList;
    
	if (GetNetMode() != NM_Client)
	{
		OnRep_PlayerList();
	}
}

void ALobbyGameState::OnRep_PlayerList()
{
	if (ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>())
	{
		if (GS->OnPlayerListChanged.IsBound())
		{
			GS->OnPlayerListChanged.Broadcast();
		}
	}
}

void ALobbyGameState::OnRep_LobbyAdmin()
{
	if (OnLobbyAdminChangedDelegate.IsBound())
	{
		OnLobbyAdminChangedDelegate.Broadcast();
	}
}

TArray<FString> ALobbyGameState::GetPlayerNames()
{
	TArray<FString> Names;
	for (APlayerState* PS : PlayerArray)
	{
		if (PS)
		{
			FString PName = PS->GetPlayerName();
			if (PName.IsEmpty())
			{
				Names.Add(TEXT("Loading...")); 
			}
			else
			{
				Names.Add(PName);
			}
		}
	}
	return Names;
}
