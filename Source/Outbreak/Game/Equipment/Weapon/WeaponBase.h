// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Game/Equipment/EquipmentBase.h"
#include "WeaponBase.generated.h"

UCLASS(Abstract)
class OUTBREAK_API AWeaponBase : public AEquipmentBase
{
	GENERATED_BODY()
	
// --------------------
// Functions
// --------------------	
public:
	AWeaponBase();
	virtual void OnEquip() override PURE_VIRTUAL(AWeaponBase::OnEquip, );
	virtual void OnUse() override PURE_VIRTUAL(AWeaponBase::OnUse, );
	virtual void OnEndUse() override PURE_VIRTUAL(AWeaponBase::OnEndUse, );
	virtual bool CanUse() const override PURE_VIRTUAL(AWeaponBase::CanUse, return true;);

	EWeaponType GetWeaponType() const { return WeaponType; }
	float GetBaseDamage() const { return WeaponData.Damage; }

protected:
	virtual void DealDamage(AActor* Target, float Damage);

// --------------------
// Variables
// --------------------
protected:
	EWeaponType WeaponType;
	FWeaponData WeaponData;
};