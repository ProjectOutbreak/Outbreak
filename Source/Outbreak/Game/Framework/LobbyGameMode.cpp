#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameLiftServerSDK.h"
#include "OutbreakGameLiftSubsystem.h"
#include "Misc/PackageName.h"
ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	ConnectedPlayers = 0;
	UE_LOG(LogTemp, Warning , TEXT("대기방 시작"));
}

// 대기방 접속
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ConnectedPlayers++;

	UE_LOG(LogTemp, Warning, TEXT("플레이어 접속 : 현재 %d명"), ConnectedPlayers);
	StartMatchIfReady();
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (ConnectedPlayers > 0)
	{
		ConnectedPlayers--;
	}
	UE_LOG(LogTemp,Warning,TEXT("플레이어 퇴장. 현재 인원: %d명"),ConnectedPlayers);

	if (ConnectedPlayers <= 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("[LobbyGameMode] No Players in Lobby Session... Terminating Server"));
		auto* GI = GetGameInstance();
		if (GI)
		{
			UOutbreakGameLiftSubsystem* GameLiftSys = GI->GetSubsystem<UOutbreakGameLiftSubsystem>();
			if (GameLiftSys)
			{
				GameLiftSys->EndGameServer();
			}
		}
	}
}

void ALobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                              FString& ErrorMessage)
{
	if (ConnectedPlayers >= 4)
	{
		ErrorMessage = TEXT("대기방 인원이 가득 찼습니다. 접속이 거부되었습니다.");
		UE_LOG(LogTemp, Warning, TEXT("접속 거부: 최대 인원 초과"));
		return;
	}
// Check Client Ticket 
#if WITH_GAMELIFT
	FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));

	if (!PlayerSessionId.IsEmpty())
	{
		FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
        
		FGameLiftGenericOutcome Outcome = GameLiftSdkModule->AcceptPlayerSession(PlayerSessionId);
		if (Outcome.IsSuccess())
		{
			UE_LOG(LogTemp, Display, TEXT("[PreLogin] GameLift 승인 성공! 유저 입장 허용."));
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
		UE_LOG(LogTemp, Warning, TEXT("[PreLogin] 입장권 없음. 로컬 테스트로 간주하고 입장 허용."));
	}
#endif
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void ALobbyGameMode::StartMatchIfReady()
{
	// TODO : 대기방에 접속한 인원이 4명일 뿐아니라 4명 모두 캐릭터 선택 완료시에 게임 시작하도록 조건 추가
	if (!HasMatchStarted() && ConnectedPlayers == 4)
	{
		if (HasAuthority())
		{
			FString TargetMapPath = TEXT("/Game/Maps/L_FirstPhase");
			if (FPackageName::DoesPackageExist(TargetMapPath))
			{
				StartMatch(); // MatchState = InProgress
				UE_LOG(LogTemp, Warning, TEXT("매치 시작 조건 만족"));
				FString TravelURL = FString::Printf(TEXT("%s?listen"), *TargetMapPath);
                
				GetWorld()->ServerTravel(TravelURL, true);
			}
			else
            {
                UE_LOG(LogTemp, Error, TEXT("CRITICAL: 이동하려는 맵을 찾을 수 없습니다! 경로: %s"), *TargetMapPath);
            }
		}
	}
}