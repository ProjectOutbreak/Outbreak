// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableBase.h"

AThrowableBase::AThrowableBase()
{
}

void AThrowableBase::Attack()
{
	Super::Attack();
}

bool AThrowableBase::CanAttack() const
{
	return Super::CanAttack();
}

void AThrowableBase::Throw()
{
}

bool AThrowableBase::CanThrow() const
{
	return true;
}

void AThrowableBase::AddCount(int32 Amount)
{
}

void AThrowableBase::SpawnProjectile()
{
}

FVector AThrowableBase::CalculateThrowVelocity()
{
	return FVector::ZeroVector;
}
