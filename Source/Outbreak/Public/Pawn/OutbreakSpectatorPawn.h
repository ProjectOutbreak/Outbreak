// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "OutbreakSpectatorPawn.generated.h"

UCLASS()
class OUTBREAK_API AOutbreakSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
	
public:
	AOutbreakSpectatorPawn();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Server, Reliable)
	void ServerNextPlayer();

	UFUNCTION(Server, Reliable)
	void ServerPrevPlayer();

	UFUNCTION(Server, Reliable)
	void ServerToggleFreeCam();

private:
	void UpdateSpectateTarget(int32 Direction);
    
	int32 CurrentTargetIndex = -1;
};