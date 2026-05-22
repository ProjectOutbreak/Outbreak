// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMode.h"
#include "EasySessionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Containers/Set.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Manager/CharacterSpawnManager.h"
#include "Outbreak/Manager/SoundManager.h"
#include "Pawn/OutbreakSpectatorPawn.h"
#include "OutbreakGameLiftSubsystem.h"
#include "Data/OutbreakDeveloperSettings.h"
#include "Utilities/DebugHelper.h"

AInGameMode::AInGameMode()
{
	bUseSeamlessTravel = true;
}

void AInGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (USoundManager* SM = GetGameInstance()->GetSubsystem<USoundManager>())
	{
		SM->StartMainBgmShuffle(false, 0.6f);
	}
}

void AInGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
}

void AInGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	DelayedRefreshSpawnManagerTargets();

	if (bIsServerShuttingDown) return;

	if (ACharacterPlayer* ExitingCharacter = Cast<ACharacterPlayer>(Exiting->GetPawn()))
	{
		if (ExitingCharacter->GetEquipmentController())
		{
			ExitingCharacter->GetEquipmentController()->DestroyAllEquipment();
		}
		ExitingCharacter->Destroy();
	}
	if (IsRunningDedicatedServer())
	{
		int32 RemainingPlayers = 0;
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (APlayerController* PC = Iterator->Get())
			{
				if (PC != Exiting) RemainingPlayers++; 
			}
		}
		if (RemainingPlayers <= 0)
		{
			if (const UGameInstance* GI = GetGameInstance())
			{
				if (UOutbreakGameLiftSubsystem* GameLiftSys = GI->GetSubsystem<UOutbreakGameLiftSubsystem>())
				{
					PRINT_WITH_CURRENT_CONTEXT(TEXT("[AWS] Empty Server Detected. Shutting down GameLift Instance..."));
					GameLiftSys->EndGameServer();
				}
			}
			return;
		}
	}
	if (IsGameOver())
	{
		GameOver();
	}
}


void AInGameMode::OnPlayerDie(ACharacter* DeadCharacter, AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->UnPossess();

		if (OutbreakSpectatorClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = PC;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			const FVector SpawnLocation = DeadCharacter->GetActorLocation() + FVector(0, 0, 500);
			const FRotator SpawnRotation = DeadCharacter->GetActorRotation();

			if (AOutbreakSpectatorPawn* NewSpectator = GetWorld()->SpawnActor<AOutbreakSpectatorPawn>(OutbreakSpectatorClass, SpawnLocation, SpawnRotation, SpawnParams))
			{
				PC->Possess(NewSpectator);
			}
		}
	}
	
	if (IsGameOver())
	{
		GameOver();
	}
}

void AInGameMode::GameCleared()
{
	FString LobbyMapPath = TEXT("/Game/Maps/L_Lobby?listen");
	GetWorld()->ServerTravel(LobbyMapPath);
}	

void AInGameMode::DelayedRefreshSpawnManagerTargets()
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
          
	TimerDelegate.BindUObject(this, &AInGameMode::RefreshSpawnManagerTargets);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 5.0f, false);
}

void AInGameMode::RefreshSpawnManagerTargets()
{
	if (!SpawnManagerInstance)
	{
		InstantiateSpawnManager();
	}

	TArray<AActor*> PlayerPawns;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const APlayerController* PC = It->Get())
		{
			if (APawn* P = PC->GetPawn())
			{
				PlayerPawns.Add(P);
			}
		}
	}

	if (PlayerPawns.Num() > 0)
	{
		SpawnManagerInstance->UpdateActivePlayers(PlayerPawns);
        
		const UOutbreakDeveloperSettings* Settings = UOutbreakDeveloperSettings::Get();
		if (!SpawnManagerInstance->IsActivated() && Settings->bAutoActivateSpawnManager)
		{
			SpawnManagerInstance->Activate();
		}
	}
	else
	{
		SpawnManagerInstance->Deactivate();
	}
}


bool AInGameMode::IsGameOver() const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const APlayerController* PC = It->Get())
		{
			const ACharacterPlayer* PlayerCharacter = Cast<ACharacterPlayer>(PC->GetPawn());
            
			if (IsValid(PlayerCharacter) && !PlayerCharacter->IsActorBeingDestroyed() && !PlayerCharacter->IsDead())
			{
				return false;
			}
		}
	}
	
	return true;
}

void AInGameMode::ProcessPlayerQuit(APlayerController* ExitingPlayer)
{
	if (!ExitingPlayer) return;
	if (GetNetMode() == NM_ListenServer && ExitingPlayer->IsLocalController())
	{
		PRINT_WITH_CURRENT_CONTEXT(TEXT("Listen Server Host is quitting. Shutting down server..."));
		
		bIsServerShuttingDown = true;
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UEasySessionSubsystem* EasySession = GI->GetSubsystem<UEasySessionSubsystem>())
			{
				EasySession->DestroySession();
			}
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (PC && PC != ExitingPlayer)
			{
				PC->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("Host has closed the server.")));
			}
		}
	}
	ExitingPlayer->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("You left the match.")));
}

void AInGameMode::OnPlayerReady()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(OnPlayerReady);
	ReadyPlayersCount++;

	int32 TotalPlayers = GetNumPlayers();
	
	if (ReadyPlayersCount >= TotalPlayers)
	{
		RefreshSpawnManagerTargets(); 
	}
}

void AInGameMode::ProcessGameOverSequence()
{
	if (IsRunningDedicatedServer())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				PC->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("Game Over. Returning to Main Menu.")));
			}
		}

		if (const UGameInstance* GI = GetGameInstance())
		{
			if (UOutbreakGameLiftSubsystem* GameLiftSys = GI->GetSubsystem<UOutbreakGameLiftSubsystem>())
			{
				PRINT_WITH_CURRENT_CONTEXT(TEXT("[AWS] Match Finished. Shutting down GameLift Instance..."));
				GameLiftSys->EndGameServer();
			}
		}
	}
	else
	{
		FString LobbyMap = "/Game/Maps/L_Lobby?listen";
		GetWorld()->ServerTravel(LobbyMap);
	}
}

void AInGameMode::GameOver()
{
	PRINT_WITH_CURRENT_CONTEXT(TEXT("Game Over!"));
	if (bHasGameOverTriggered) return;
	bHasGameOverTriggered = true;
	if (SpawnManagerInstance)
	{
		SpawnManagerInstance->Deactivate();
	}

	GetWorld()->GetTimerManager().SetTimer(GameOverTimerHandle, this, &AInGameMode::ProcessGameOverSequence, 5.0f, false);
}

void AInGameMode::InstantiateSpawnManager()
{
	if (!SpawnManagerInstance && SpawnManagerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnManagerInstance = GetWorld()->SpawnActor<ACharacterSpawnManager>(SpawnManagerClass, SpawnParams);
	}
}