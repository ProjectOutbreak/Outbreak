// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/ZombieMassFragments.h"

FZombieHealthFragment::FZombieHealthFragment()
	: CurrentHealth(100.f)
{
}

FZombieWanderFragment::FZombieWanderFragment() 
	: Origin(FVector::ZeroVector) 
	, TargetLocation(FVector::ZeroVector)
	, TimeUntilNewTarget(0)
	, Radius(1000.0f)
	, Speed(100.0f)
{
}

FZombieDeathFragment::FZombieDeathFragment() 
	: TimeToLive(5.0f)
{
}
