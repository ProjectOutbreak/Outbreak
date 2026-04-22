// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "ZombieHealthFragment.generated.h"

USTRUCT()
struct OUTBREAK_API FZombieHealthFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieHealthFragment();

	UPROPERTY()
	float CurrentHealth;

	UPROPERTY()
	float MaxHealth;

	UPROPERTY()
	bool bIsDead;
	
};
