// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "ActiveAbility.h"
#include "BaseActiveAbility.generated.h"

UCLASS()
class OUTBREAK_API UBaseActiveAbility : public UBaseAbility, public IActiveAbility
{
	GENERATED_BODY()

public:
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual bool CanActivate() const override { return GetRemainingCooldown() <= 0.0f && !bIsActive; }

	virtual void OnActivate() PURE_VIRTUAL(OnActivate, );
	virtual void OnDeactivate() PURE_VIRTUAL(OnDeactivate, );

	virtual float GetCooldown() const { return Cooldown; }
	virtual float GetRemainingCooldown() const { return RemainingCooldown; }
	
	virtual float GetDuration() const { return Duration; }
	virtual float GetRemainingDuration() const { return RemainingDuration; }
	
	virtual bool IsAbilityActive() const { return bIsActive; }

	void TickCooldown(float DeltaTime);
	void TickDuration(float DeltaTime);

protected:
	float Cooldown = 0.0f;
	float Duration = 0.0f;
	float RemainingCooldown = 0.0f;
	float RemainingDuration = 0.0f;
	bool bIsActive = false;
};