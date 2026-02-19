// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialZombie.h"
#include "BoomerZombie.generated.h"

UCLASS()
class OUTBREAK_API ABoomerZombie : public ASpecialZombie
{
	GENERATED_BODY()

protected:
	virtual void InitCharacterData() override;
	virtual void OnRep_Die() override;
};