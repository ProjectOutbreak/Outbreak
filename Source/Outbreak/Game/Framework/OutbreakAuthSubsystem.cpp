// Fill out your copyright notice in the Description page of Project Settings.


#include "OutbreakAuthSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

FString UOutbreakAuthSubsystem::GetSteamId() const
{
	if (IsRunningDedicatedServer())
	{
		UE_LOG(LogTemp, Error, TEXT("Dedicated Server Skipping SteamAuth"))
		return TEXT("Server_No_SteamID");
	}
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(FName("Steam")))
	{
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
		if (Identity.IsValid() && Identity->GetUniquePlayerId(0).IsValid())
		{
			return Identity->GetUniquePlayerId(0)->ToString();
		}
	}
	return TEXT("");
}

