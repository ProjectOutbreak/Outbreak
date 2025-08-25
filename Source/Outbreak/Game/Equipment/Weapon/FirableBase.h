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
	virtual void OnUse() override { StartFire(); }
	virtual void OnEndUse() override { StopFire(); }
	virtual bool CanUse() const override { return !bIsInUse && !bIsReloading && CurrentAmmoInMag > 0; }

	void StartFire();
	void StopFire();
	void Reload();

	bool IsReloading() const { return bIsReloading; }
	int32 GetCurrentAmmoInMag() const { return CurrentAmmoInMag; }
	int32 GetCurrentTotalAmmo() const { return CurrentTotalAmmo; }
	FFirableData GetFirableData() const { return FirableData; }
	void SetFireType(const EFireType NewFireType) { CurrentFireType = NewFireType; }

protected:
	virtual void ProcessFire();
	virtual void SpawnProjectile();

	virtual void StartReload();
	virtual void FinishReload();
	
// --------------------
// Variables
// --------------------
protected:
	FFirableData FirableData;
	EFirableType FirableType;
	int32 CurrentAmmoInMag = 0;
	int32 CurrentTotalAmmo = 0;
	EFireType CurrentFireType = EFireType::Auto;
	bool bIsReloading = false;
	FTimerHandle FireTimerHandle;
};
