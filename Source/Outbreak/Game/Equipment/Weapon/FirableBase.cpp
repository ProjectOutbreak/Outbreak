// Fill out your copyright notice in the Description page of Project Settings.


#include "FirableBase.h"

AFirableBase::AFirableBase()
{
}

void AFirableBase::Attack()
{
	Super::Attack();
}

void AFirableBase::StartAttack()
{
	Super::StartAttack();
}

void AFirableBase::StopAttack()
{
	Super::StopAttack();
}

bool AFirableBase::CanAttack() const
{
	return Super::CanAttack();
}

void AFirableBase::Fire()
{
}

void AFirableBase::Reload()
{
}

bool AFirableBase::NeedsReload() const
{
	return false;
}

void AFirableBase::AddAmmo(int32 Amount)
{
}

void AFirableBase::ProcessFire()
{
}

void AFirableBase::SpawnProjectile()
{
}

void AFirableBase::StartReload()
{
}

void AFirableBase::FinishReload()
{
}

void AFirableBase::OnFireRateTimer()
{
}

void AFirableBase::OnReloadComplete()
{
}
