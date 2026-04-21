// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialZombie.h"
#include "PopperZombie.generated.h"

UCLASS()
class OUTBREAK_API APopperZombie : public ASpecialZombie
{
	GENERATED_BODY()

protected:
	virtual void InitCharacterData() override;
	virtual void OnRep_Die() override;
};