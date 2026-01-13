#include "LobbyPlayerController.h"
#include "Outbreak/Game/Framework/StagingGameMode.h"

void ALobbyPlayerController::RequestStartGame()
{
	Server_RequestStartGame();
}

void ALobbyPlayerController::Server_RequestStartGame_Implementation()
{
	if (AStagingGameMode* GM = Cast<AStagingGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->RequestStartGame();
	}
}