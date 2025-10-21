// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "PassiveAbility.h"
#include "BasePassiveAbility.generated.h"

UCLASS()
class OUTBREAK_API UBasePassiveAbility : public UBaseAbility, public IPassiveAbility
{
	GENERATED_BODY()

public:
	virtual void OnEquip() override PURE_VIRTUAL(OnEquip, );
	virtual void OnUnequip() override PURE_VIRTUAL(OnUnequip, );
};