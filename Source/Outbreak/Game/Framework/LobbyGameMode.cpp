#include "LobbyGameMode.h"
#include "Utilities/DebugHelper.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
}

void ALobbyGameMode::StartGame() const
{
	const FString MapName = "L_Loading";
	const FString Options = "?listen";
	
	UWorld* World = GetWorld();
	if (!World->ServerTravel(MapName + Options))
	{
		PRINT_WITH_CURRENT_CONTEXT("ServerTravel failed");
	}
}
