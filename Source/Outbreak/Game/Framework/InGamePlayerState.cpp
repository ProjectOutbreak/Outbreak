// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/UI/InGameHUD.h"

AInGamePlayerState::AInGamePlayerState()
{
	ZombieKills = 0;
}

void AInGamePlayerState::OnRep_ZombieKills()
{
	UE_LOG(LogTemp, Log, TEXT("좀비 처치 수 변경: %d"), ZombieKills);
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController() && PC->PlayerState == this)
		{
			if (AInGameHUD* HUD = Cast<AInGameHUD>(PC->GetHUD()))
			{
				HUD->DisplayZombieKills(ZombieKills);
			}
		}
	}
}

void AInGamePlayerState::AddZombieKill()
{
	ZombieKills++;
}

void AInGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AInGamePlayerState, ZombieKills);
}
