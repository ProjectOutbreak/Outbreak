// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PassiveAbility.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPassiveAbility : public UInterface
{
	GENERATED_BODY()
};

class OUTBREAK_API IPassiveAbility
{
	GENERATED_BODY()

public:
	virtual void OnEquip() = 0;
	virtual void OnUnequip() = 0;
};
