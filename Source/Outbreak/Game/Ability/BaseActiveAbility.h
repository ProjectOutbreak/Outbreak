// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "BaseActiveAbility.generated.h"

UCLASS()
class OUTBREAK_API UBaseActiveAbility : public UBaseAbility
{
	GENERATED_BODY()

public:
	virtual void Activate();
	virtual void Deactivate();

	virtual float GetCooldown() const { return Cooldown; }
	virtual float GetRemainingCooldown() const { return RemainingCooldown; }
	
	virtual float GetDuration() const { return Duration; }
	virtual float GetRemainingDuration() const { return RemainingDuration; }
	
	virtual bool IsAbilityActive() const { return bIsActive; }
	virtual bool CanUseAbility() const { return GetRemainingCooldown() <= 0.f && !bIsActive; }

	void TickCooldown(float DeltaTime);
	void TickDuration(float DeltaTime);

protected:
	float Cooldown = 0.0f;
	float Duration = 0.0f;
	float RemainingCooldown = 0.0f;
	float RemainingDuration = 0.0f;
	bool bIsActive = false;
};
