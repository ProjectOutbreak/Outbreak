// Fill out your copyright notice in the Description page of Project Settings.


#include "OutbreakAuthSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

void UOutbreakAuthSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	
	Super::Initialize(Collection);
}

void UOutbreakAuthSubsystem::Deinitialize()
{
	SteamIdentityInterface = nullptr;
	Super::Deinitialize();
}

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

FString UOutbreakAuthSubsystem::GetSteamAuthTicket()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(FName("Steam"));
	if (Subsystem && Subsystem->GetIdentityInterface().IsValid())
	{
		return Subsystem->GetIdentityInterface()->GetAuthToken(0);
	}
	return TEXT("");
}
