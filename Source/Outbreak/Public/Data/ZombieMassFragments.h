// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "ZombieMassFragments.generated.h"

enum class EZombieStateType : uint8;

USTRUCT()
struct OUTBREAK_API FZombieHealthFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieHealthFragment();

	UPROPERTY(VisibleAnywhere)
	int32 CurrentHealth;
};

USTRUCT()
struct FZombieWanderFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieWanderFragment();
	
	UPROPERTY(VisibleAnywhere)
	FVector Origin;
	
	UPROPERTY(VisibleAnywhere)
	FVector TargetLocation;
	
	UPROPERTY(VisibleAnywhere)
	float TimeUntilNewTarget;
	
	UPROPERTY(VisibleAnywhere)
	float Radius;
	
	UPROPERTY(VisibleAnywhere)
	float Speed;
};

USTRUCT()
struct FZombieDeathFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieDeathFragment();
	
	float TimeToLive;
};