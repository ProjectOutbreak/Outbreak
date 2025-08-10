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
	virtual void OnEquip(TObjectPtr<class ACharacterPlayer> Character) override;
	virtual void OnUnequip() override;
	virtual void OnUse() override { StartAttack(); }
	virtual bool CanUse() const override { return CanAttack(); }

	virtual void Attack() PURE_VIRTUAL(AWeaponBase::Attack, );
	virtual void StartAttack() { Attack(); }
	virtual void StopAttack() { bIsAttacking = false; }
	virtual bool CanAttack() const { return !bIsAttacking; }

	EWeaponType GetWeaponType() const { return WeaponType; }
	float GetBaseDamage() const { return WeaponData.Damage; }

protected:
	virtual void PlayAttackEffects();
	virtual void DealDamage(AActor* Target, float Damage);

// --------------------
// Variables
// --------------------
protected:
	EWeaponType WeaponType;
	FWeaponData WeaponData;
	bool bIsAttacking = false;

	UPROPERTY()
	TObjectPtr<class UNiagaraSystem> AttackEffect;
	
	UPROPERTY()
	TObjectPtr<class USoundCue> AttackSound;
};