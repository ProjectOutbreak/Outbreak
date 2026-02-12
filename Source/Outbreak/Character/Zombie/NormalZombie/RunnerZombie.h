// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NormalZombie.h"
#include "RunnerZombie.generated.h"

UCLASS()
class OUTBREAK_API ARunnerZombie : public ANormalZombie
{
	GENERATED_BODY()

public:
	ARunnerZombie();
	
protected:
	virtual void BeginPlay() override;
};