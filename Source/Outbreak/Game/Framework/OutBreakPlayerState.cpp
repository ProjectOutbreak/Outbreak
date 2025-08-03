// Fill out your copyright notice in the Description page of Project Settings.


#include "OutBreakPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/UI/OBHUD.h"

AOutBreakPlayerState::AOutBreakPlayerState()
{
	ZombieKills = 0;
}

void AOutBreakPlayerState::OnRep_ZombieKills()
{
	UE_LOG(LogTemp, Log, TEXT("좀비 처치 수 변경: %d"), ZombieKills);
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController() && PC->PlayerState == this)
		{
			if (AOBHUD* HUD = Cast<AOBHUD>(PC->GetHUD()))
			{
				HUD->DisplayZombieKills(ZombieKills);
			}
		}
	}
}

void AOutBreakPlayerState::AddZombieKill()
{
	ZombieKills++;
}

void AOutBreakPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AOutBreakPlayerState, ZombieKills);
}
