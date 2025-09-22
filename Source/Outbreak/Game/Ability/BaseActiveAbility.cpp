// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseActiveAbility.h"

void UBaseActiveAbility::Activate()
{
	if (!CanActivate())
		return;

	RemainingCooldown = Cooldown;
	RemainingDuration = Duration;
	bIsActive = true;

	OnActivate();
}

void UBaseActiveAbility::Deactivate()
{
	RemainingDuration = 0.0f;
	bIsActive = false;

	OnDeactivate();
}

void UBaseActiveAbility::TickCooldown(float DeltaTime)
{
	if (RemainingCooldown > 0.0f)
	{
		RemainingCooldown = FMath::Max(RemainingCooldown - DeltaTime, 0.0f);
	}
}

void UBaseActiveAbility::TickDuration(float DeltaTime)
{
	if (bIsActive)
	{
		RemainingDuration = FMath::Max(RemainingDuration - DeltaTime, 0.0f);
		if (RemainingDuration <= 0.0f)
		{
			Deactivate();
			bIsActive = false;
		}
	}
}
