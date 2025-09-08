// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePassiveAbility.h"
#include "ToxicAttackAbility.generated.h"

UCLASS()
class OUTBREAK_API UToxicAttackAbility : public UBasePassiveAbility
{
	GENERATED_BODY()

public:
	UToxicAttackAbility();
	virtual void OnEquip() override;
	virtual void OnUnequip() override;

protected:
	UFUNCTION()
	void HandleOwnerAttackHit(AActor* HitActor, const FHitResult& HitResult);

private:
	float ToxicDamagePerSecond = 1.0f;
	float ToxicDuration = 10.0f;
};
