// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "FirableBase.generated.h"

UCLASS(Abstract)
class OUTBREAK_API AFirableBase : public AWeaponBase
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------	
public:
	AFirableBase();
	virtual void Attack() override;
	virtual void StartAttack() override;
	virtual void StopAttack() override;
	virtual bool CanAttack() const override;

	virtual void Fire();
	virtual void Reload();

	bool NeedsReload() const;
	bool IsReloading() const { return bIsReloading; }

	int32 GetCurrentAmmoInMag() const { return CurrentAmmoInMag; }
	int32 GetCurrentTotalAmmo() const { return CurrentTotalAmmo; }
	void AddAmmo(int32 Amount);

protected:
	virtual void ProcessFire();
	virtual void SpawnProjectile();

	virtual void StartReload();
	virtual void FinishReload();

	UFUNCTION()
	void OnFireRateTimer();

	UFUNCTION()
	void OnReloadComplete();

// --------------------
// Variables
// --------------------
protected:
	FFirableData FirableData;
	EFirableType FirableType;
	int32 CurrentAmmoInMag = 0;
	int32 CurrentTotalAmmo = 0;
	bool bIsReloading = false;
	bool bCanFire = true;
	FTimerHandle ReloadTimerHandle;
	FTimerHandle ReloadTimer;
	
	UPROPERTY()
	TObjectPtr<class UNiagaraSystem> MuzzleFlash;

	UPROPERTY()
	TObjectPtr<class USoundCue> FireSound;
};
