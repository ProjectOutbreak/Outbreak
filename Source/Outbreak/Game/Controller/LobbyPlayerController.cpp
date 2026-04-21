#include "LobbyPlayerController.h"

#include "EasySessionSubsystem.h"
#include "Outbreak/Game/Framework/StagingGameMode.h"

void ALobbyPlayerController::RequestStartGame()
{
	Server_RequestStartGame();
}

void ALobbyPlayerController::LeaveLobby()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UEasySessionSubsystem* EasySession = GI->GetSubsystem<UEasySessionSubsystem>())
		{
			EasySession->DestroySession();
		}
	}
	Server_LeaveLobby();
}

void ALobbyPlayerController::Server_LeaveLobby_Implementation()
{
	if (AStagingGameMode* GM = Cast<AStagingGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->ProcessPlayerQuit(this); 
	}
}

void ALobbyPlayerController::Server_RequestStartGame_Implementation()
{
	if (AStagingGameMode* GM = Cast<AStagingGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->RequestStartGame();
	}
}
