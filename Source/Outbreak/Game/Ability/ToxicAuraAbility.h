// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePassiveAbility.h"
#include "ToxicAuraAbility.generated.h"

UCLASS()
class OUTBREAK_API UToxicAuraAbility : public UBasePassiveAbility
{
	GENERATED_BODY()

public:
	UToxicAuraAbility();
	virtual void OnEquip() override;
	virtual void OnUnequip() override;

protected:
	void OnToxicAuraAbility();
	
private:
	UPROPERTY()
	TSet<AActor*> AffectedPlayers;
	
	FTimerHandle ToxicAuraTimerHandle;
	float AuraRadius = 1000.0f;
	float CheckInterval = 0.5f;
	
};