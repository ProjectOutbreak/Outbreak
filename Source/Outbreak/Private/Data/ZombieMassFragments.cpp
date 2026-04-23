// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ZombieMassFragments.h"

#include "Util/Define.h"

FZombieEntityFragment::FZombieEntityFragment()
	: Position(FVector::ZeroVector),
	  Forward(FVector::ForwardVector),
	  TimeToLive(0.f)
{
}

FZombieHealthFragment::FZombieHealthFragment()
	: CurrentHealth(100.f),
	  MaxHealth(100.f),
	  bIsDead(false)
{
}

FZombieStateFragment::FZombieStateFragment()
	: CurrentState(EZombieStateType::Idle),
	  bIsAlert(false)
{
}

FZombieMovementFragment::FZombieMovementFragment()
	: MaxWanderSpeed(100.0f)
	, MaxRunSpeed(500.0f)
{
}

FZombieCombatFragment::FZombieCombatFragment()
	: AttackDamage(10)
	, AttackRange(300.0f)
	, AttackRate(1.0f)
{
}

FZombiePerceptionFragment::FZombiePerceptionFragment()
	: SightRadius(2000.0f)
	, LoseSightRadius(2500.0f)
	, PeripheralVisionAngleDegrees(90.0f)
{
}

FZombieChaseTargetSharedFragment::FZombieChaseTargetSharedFragment()
	: TargetLocations()
{
}

FZombieChaseTargetFragment::FZombieChaseTargetFragment() 
	: TargetIndex(0)
{
}

FZombieWanderFragment::FZombieWanderFragment() 
	: TimeUntilNewTarget(0)
	, Radius(1000.0f)
	, Speed(100.0f)
{
}

FZombieDeathFragment::FZombieDeathFragment() 
	: TimeToLive(5.0f)
{
}
