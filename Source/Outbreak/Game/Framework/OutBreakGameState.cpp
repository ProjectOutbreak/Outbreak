// Fill out your copyright notice in the Description page of Project Settings.


#include "OutBreakGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/UI/OBHUD.h"

AOutBreakGameState::AOutBreakGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	TotalZombieKills = 0;
	AlivePlayerCount = 0;
}

void AOutBreakGameState::BeginPlay()
{
	Super::BeginPlay();

	FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
	if (CurrentLevel == TEXT("FirstPhase")) CurrentPhase = "LEVEL 1 : Lush Forest";
	else if (CurrentLevel == TEXT("SecondPhase")) CurrentPhase = "LEVEL 2 : Devastated Village";
	else if (CurrentLevel == TEXT("ThirdPhase")) CurrentPhase = "LEVEL 3 : Skyscrapers";
	else if (CurrentLevel == TEXT("LastPhase")) CurrentPhase = "LEVEL 4 : Last Forest";
}

void AOutBreakGameState::Tick(float DeltaTime)
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

void AOutBreakGameState::OnRep_TotalZombieKills()
{
	UE_LOG(LogTemp, Log, TEXT("총 좀비 처치 수 변경: %d"), TotalZombieKills);
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController())
		{
			if (AOBHUD* HUD = Cast<AOBHUD>(PC->GetHUD()))
			{
				HUD->DisplayTotalZombieKills(TotalZombieKills);
			}
		}
	}
}

void AOutBreakGameState::AddTotalZombieKill()
{
	TotalZombieKills++;
	OnRep_TotalZombieKills();
}


void AOutBreakGameState::OnRep_AlivePlayerCount()
{
	UE_LOG(LogTemp, Log, TEXT("생존 플레이어 수 변경: %d"), AlivePlayerCount);
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController())
		{
			if (AOBHUD* HUD = Cast<AOBHUD>(PC->GetHUD()))
			{
				HUD->DisplayAlivePlayerCount(AlivePlayerCount);
			}	
		}
	}
}

void AOutBreakGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOutBreakGameState, MatchTime);
	DOREPLIFETIME(AOutBreakGameState, CurrentPhase);
	DOREPLIFETIME(AOutBreakGameState, TotalZombieKills);
	DOREPLIFETIME(AOutBreakGameState, AlivePlayerCount);
}




