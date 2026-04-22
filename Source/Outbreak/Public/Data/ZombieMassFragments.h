// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "ZombieMassFragments.generated.h"

enum class EZombieStateType : uint8;

USTRUCT()
struct FZombieEntityFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieEntityFragment();
	
	FVector Position;
	FVector Forward;
	float TimeToLive;
};

USTRUCT()
struct OUTBREAK_API FZombieHealthFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieHealthFragment();

	UPROPERTY(VisibleAnywhere)
	int32 CurrentHealth;

	UPROPERTY(VisibleAnywhere)
	int32 MaxHealth;

	UPROPERTY(VisibleAnywhere)
	bool bIsDead;
	
};

USTRUCT()
struct OUTBREAK_API FZombieStateFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieStateFragment();

	UPROPERTY(VisibleAnywhere)
	EZombieStateType CurrentState;

	UPROPERTY(VisibleAnywhere)
	bool bIsAlert;
};

USTRUCT()
struct FZombieMovementFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieMovementFragment();
	
	float MaxWanderSpeed;
	float MaxRunSpeed;
};

USTRUCT()
struct FZombieCombatFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieCombatFragment();
	
	int32 AttackDamage;
	float AttackRange;
	float AttackRate;
};

USTRUCT()
struct FZombiePerceptionFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombiePerceptionFragment();
	
	float SightRadius;
	float LoseSightRadius;
	float PeripheralVisionAngleDegrees;
};

USTRUCT()
struct FZombieChaseTargetSharedFragment : public FMassSharedFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieChaseTargetSharedFragment();
	
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> TargetLocations;
	
};

USTRUCT()
struct FZombieChaseTargetFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieChaseTargetFragment();
	
	int32 TargetIndex; 
};

USTRUCT()
struct FZombieWanderFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieWanderFragment();
	
	FVector Origin;
	FVector TargetLocation;
	float TimeUntilNewTarget;
	float Radius;
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