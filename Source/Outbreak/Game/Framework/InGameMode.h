// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "InGameMode.generated.h"


UCLASS()
class OUTBREAK_API AInGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	AInGameMode();

	UFUNCTION()
	void ProceedToNextLevel() const;
};
