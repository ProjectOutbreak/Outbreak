// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMode.h"

#include "OutBreakGameState.h"
#include "OutBreakPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Containers/Set.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/UI/OBHUD.h"


AInGameMode::AInGameMode()
{
	DefaultPawnClass = ACharacterPlayer::StaticClass();
	HUDClass = AOBHUD::StaticClass();
	GameStateClass = AOutBreakGameState::StaticClass();
	PlayerStateClass = AOutBreakPlayerState::StaticClass();

	bUseSeamlessTravel = true;
}
void AInGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartMatch();
	UE_LOG(LogTemp, Warning, TEXT("게임 시작됨"));
}

void AInGameMode::ProceedToNextLevel() const
{
	if (!HasAuthority()) return;

	FString NextLevelName;

	FName CurrentLevel = *UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

	if (CurrentLevel == TEXT("FirstPhase"))
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