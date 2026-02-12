// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NormalZombie.h"
#include "BloaterZombie.generated.h"

UCLASS()
class OUTBREAK_API ABloaterZombie : public ANormalZombie
{
	GENERATED_BODY()

public:
	ABloaterZombie();
	
protected:
	virtual void BeginPlay() override;
};
