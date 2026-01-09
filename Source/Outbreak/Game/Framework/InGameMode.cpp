// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Containers/Set.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Manager/CharacterSpawnManager.h"
#include "Outbreak/Manager/SoundManager.h"
#include "Pawn/OutbreakSpectatorPawn.h"
#include "OutbreakGameLiftSubsystem.h"
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
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnManager = GetWorld()->SpawnActor<ACharacterSpawnManager>(SpawnManagerClass, SpawnParams);

	if (SpawnManager && !SpawnManager->IsActivated())
	{
		if (APlayerController* HostPC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
          
			TimerDelegate.BindUObject(this, &AInGameMode::ActivateSpawnManagerForPlayer, HostPC);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0f, false);
		}
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
}

void AInGameMode::ProceedToNextLevel() const
{
	if (!HasAuthority()) return;

	FString NextLevelName;

	FName CurrentLevel = *UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

	if (CurrentLevel == TEXT("L_FirstPhase"))
	{
		NextLevelName = TEXT("/Game/Maps/SecondPhase?listen");
	}
	else if (CurrentLevel == TEXT("SecondPhase"))
	{
		NextLevelName = TEXT("/Game/Maps/ThirdPhase?listen");
	}
	else if (CurrentLevel == TEXT("ThirdPhase"))
	{
		NextLevelName = TEXT("/Game/Maps/LastPhase?listen");
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("게임 종료"));
		// TODO: 대기방 레벨로 이동 코드 작성
		// 단, 마지막 페이즈는 보스 처치시 게임이 완료 됨(SafeZoneCollision이 없음)
	}
	GetWorld()->ServerTravel(NextLevelName, true);
}

void AInGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (!IsRunningDedicatedServer()) return;

	int32 RemainingPlayers = 0;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		if (PC && PC != Exiting)
		{
			RemainingPlayers++;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[InGameMode] Player Logout. Remaining: %d"), RemainingPlayers);
	if (RemainingPlayers <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InGameMode] All players left. Server Shutting Down..."));
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UOutbreakGameLiftSubsystem* GameLiftSys = GI->GetSubsystem<UOutbreakGameLiftSubsystem>())
			{
				GameLiftSys->EndGameServer();
			}
		}
	}
}

void AInGameMode::ActivateSpawnManagerForPlayer(APlayerController* PlayerToTarget)
{
	if (SpawnManager && !SpawnManager->IsActivated() && IsValid(PlayerToTarget))
	{
		if (ACharacterPlayer* TargetPlayer = Cast<ACharacterPlayer>(PlayerToTarget->GetPawn()))
		{
			SpawnManager->Activate(TargetPlayer);
		}
	}
}
