// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

AWeaponBase::AWeaponBase()
{
}

void AWeaponBase::OnEquip(TObjectPtr<class ACharacterPlayer> Character)
{
	Super::OnEquip(Character);
}

void AWeaponBase::OnUnequip()
{
	Super::OnUnequip();
}

void AWeaponBase::PlayAttackEffects()
{
}

void AWeaponBase::DealDamage(AActor* Target, float Damage)
{
}
