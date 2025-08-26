// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_DELEGATE(FOnReloadFinished);

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
	void OnReload(const FOnReloadFinished& DoneCallback);

	bool CanReload() const { return !bIsReloading && CurrentAmmoInMag < FirableData.MagazineSize && CurrentTotalAmmo > 0; }
	bool IsReloading() const { return bIsReloading; }
	int32 GetCurrentAmmoInMag() const { return CurrentAmmoInMag; }
	int32 GetCurrentTotalAmmo() const { return CurrentTotalAmmo; }
	FFirableData GetFirableData() const { return FirableData; }
	EFireType GetCurrentFireType() const { return CurrentFireType; }
	void SetFireType(const EFireType NewFireType) { CurrentFireType = NewFireType; }

protected:
	void StartFire();
	void StopFire();
	
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
	int32 CurrentAmmoInMag = 30;
	int32 CurrentTotalAmmo = 1000;
	EFireType CurrentFireType = EFireType::Auto;
	bool bIsReloading = false;
	FTimerHandle FireTimerHandle;
	
	FOnReloadFinished OnReloadFinishedCallback;
};
