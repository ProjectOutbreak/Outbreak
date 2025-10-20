// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/UI/InGameHUD.h"

AInGameState::AInGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	TotalZombieKills = 0;
	AlivePlayerCount = 0;
}

void AInGameState::BeginPlay()
{
	Super::BeginPlay();

	FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
	if (CurrentLevel == TEXT("FirstPhase")) CurrentPhase = "LEVEL 1 : Lush Forest";
	else if (CurrentLevel == TEXT("SecondPhase")) CurrentPhase = "LEVEL 2 : Devastated Village";
	else if (CurrentLevel == TEXT("ThirdPhase")) CurrentPhase = "LEVEL 3 : Skyscrapers";
	else if (CurrentLevel == TEXT("LastPhase")) CurrentPhase = "LEVEL 4 : Last Forest";
}

void AInGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		MatchTime += DeltaTime;

		int32 AliveCount = 0;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				APawn* Pawn = PC->GetPawn();
				if (Pawn && !Pawn->IsPendingKillPending())
				{
					AliveCount++;
				}
			}
		}
		if (AlivePlayerCount != AliveCount)
		{
			AlivePlayerCount = AliveCount;
			OnRep_AlivePlayerCount();
		}
	}
}

void AInGameState::OnRep_TotalZombieKills()
{
	UE_LOG(LogTemp, Log, TEXT("총 좀비 처치 수 변경: %d"), TotalZombieKills);
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController())
		{
			if (AInGameHUD* HUD = Cast<AInGameHUD>(PC->GetHUD()))
			{
				HUD->DisplayTotalZombieKills(TotalZombieKills);
			}
		}
	}
}

void AInGameState::AddTotalZombieKill()
{
	TotalZombieKills++;
	OnRep_TotalZombieKills();
}


void AInGameState::OnRep_AlivePlayerCount()
{
	UE_LOG(LogTemp, Log, TEXT("생존 플레이어 수 변경: %d"), AlivePlayerCount);
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController())
		{
			if (AInGameHUD* HUD = Cast<AInGameHUD>(PC->GetHUD()))
			{
				HUD->DisplayAlivePlayerCount(AlivePlayerCount);
			}	
		}
	}
}

void AInGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInGameState, MatchTime);
	DOREPLIFETIME(AInGameState, CurrentPhase);
	DOREPLIFETIME(AInGameState, TotalZombieKills);
	DOREPLIFETIME(AInGameState, AlivePlayerCount);
}




