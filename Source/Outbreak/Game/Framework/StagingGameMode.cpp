#include "StagingGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameLiftServerSDK.h"
#include "OutbreakGameLiftSubsystem.h"
#include "Misc/PackageName.h"
#include "Framework/GameState/LobbyGameState.h"
#include "Utilities/DebugHelper.h"

AStagingGameMode::AStagingGameMode()
{
	bUseSeamlessTravel = true;
}

void AStagingGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		UOutbreakGameLiftSubsystem* GameLiftSys = GI->GetSubsystem<UOutbreakGameLiftSubsystem>();
		if (GameLiftSys)
		{
			UE_LOG(LogTemp, Log, TEXT("[GameMode] Map Loaded. Starting GameLift Server..."));
			GameLiftSys->StartGameServer();
		}
	}
	ConnectedPlayers = 0;
	bIsTravelling = false;
	UE_LOG(LogTemp, Warning , TEXT("[StagingGameMode] 집결지(Staging) 세션 시작"));
}

void AStagingGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ConnectedPlayers++;

	UE_LOG(LogTemp, Warning, TEXT("[StagingGameMode] Connected Players : %d"), ConnectedPlayers);
}

void AStagingGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	if (ConnectedPlayers > 0)
	{
		ConnectedPlayers--;
	}
	UE_LOG(LogTemp, Warning, TEXT("[StagingGameMode] Player Logout. Connected Players: %d명"), ConnectedPlayers);

	if (ConnectedPlayers <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StagingGameMode] No Players. Terminating Server..."));
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UOutbreakGameLiftSubsystem* GameLiftSys = GI->GetSubsystem<UOutbreakGameLiftSubsystem>())
			{
				GameLiftSys->EndGameServer();
			}
		}
	}
}

void AStagingGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	if (ConnectedPlayers >= 4)
	{
		ErrorMessage = TEXT("집결지 인원이 가득 찼습니다.");
		UE_LOG(LogTemp, Warning, TEXT("접속 거부: 최대 인원 초과"));
		return;
	}

#if WITH_GAMELIFT
	FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));

	if (!PlayerSessionId.IsEmpty())
	{
		FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
		
		FGameLiftGenericOutcome Outcome = GameLiftSdkModule->AcceptPlayerSession(PlayerSessionId);
		if (Outcome.IsSuccess())
		{
			UE_LOG(LogTemp, Display, TEXT("[PreLogin] GameLift 티켓 승인 완료."));
		}
		else
		{
			ErrorMessage = TEXT("GameLift 접속 승인 실패: 유효하지 않은 세션입니다.");
			UE_LOG(LogTemp, Error, TEXT("[PreLogin] 승인 실패: %s"), *Outcome.GetError().m_errorMessage);
			return; 
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PreLogin] 티켓 없음. 로컬 테스트로 간주."));
	}
#endif

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void AStagingGameMode::RequestStartGame()
{
	if (bIsTravelling || !HasAuthority()) return; 
	
	FString TargetMapPath = TargetInGameLevel.GetLongPackageName();
	FString Options = TEXT("?listen");
			
	UWorld* World = GetWorld();
	if (!World) return;
			
	bIsTravelling = true; 
			
	if (!World->ServerTravel(TargetMapPath + Options))
	{
		PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("ServerTravel to %s failed"), *TargetMapPath));
		bIsTravelling = false;
	}
}