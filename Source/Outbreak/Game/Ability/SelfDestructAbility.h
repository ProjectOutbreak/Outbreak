// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseActiveAbility.h"
#include "SelfDestructAbility.generated.h"

UCLASS()
class OUTBREAK_API USelfDestructAbility : public UBaseActiveAbility
{
	GENERATED_BODY()

public:
	USelfDestructAbility();
	virtual void OnActivate() override;
	virtual void OnDeactivate() override;

private:
	UPROPERTY()
	TObjectPtr<class UNiagaraSystem> ExplosionEffect;

	float ExplosionRadius = 500.0f;
	float ExplosionDamage = 30.0f;
};