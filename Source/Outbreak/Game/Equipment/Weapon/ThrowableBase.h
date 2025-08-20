// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "ThrowableBase.generated.h"

UCLASS(Abstract)
class OUTBREAK_API AThrowableBase : public AWeaponBase
{
	GENERATED_BODY()
	
// --------------------
// Functions
// --------------------	
public:
	AThrowableBase();

	virtual void Attack() override;
	virtual bool CanAttack() const override;

	virtual void Throw();

	bool CanThrow() const;
	int32 GetCurrentCount() const { return CurrentCount; }
	void AddCount(int32 Amount);

protected:
	virtual void SpawnProjectile();
	virtual FVector CalculateThrowVelocity();

// --------------------
// Variables
// --------------------
protected:
	FThrowableData ThrowableData;
	EThrowableType ThrowableType;
	int32 CurrentCount = 0;
	// TSubclassOf<class AProjectileBase> ProjectileClass;

	
};
