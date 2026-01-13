#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

class AStagingGameMode;
UCLASS()
class OUTBREAK_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
    
public:
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RequestStartGame();

protected:
	UFUNCTION(Server, Reliable)
	void Server_RequestStartGame();
};