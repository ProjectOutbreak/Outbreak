// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NormalZombie.h"
#include "WalkerZombie.generated.h"

UCLASS()
class OUTBREAK_API AWalkerZombie : public ANormalZombie
{
	GENERATED_BODY()

public:
	AWalkerZombie();
	
protected:
	virtual void BeginPlay() override;
};