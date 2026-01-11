#include "LobbyPlayerController.h"
#include "Outbreak/Game/Framework/LobbyGameMode.h"

void ALobbyPlayerController::RequestStartGame()
{
	Server_RequestStartGame();
}

void ALobbyPlayerController::Server_RequestStartGame_Implementation()
{
	if (ALobbyGameMode* GM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->StartGame();
	}
}