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

	void StartFire();
	void StopFire();
	void Reload();

	bool NeedsReload() const;
	bool IsReloading() const { return bIsReloading; }

	int32 GetCurrentAmmoInMag() const { return CurrentAmmoInMag; }
	int32 GetCurrentTotalAmmo() const { return CurrentTotalAmmo; }
	FFirableData GetFirableData() const { return FirableData; }
	void AddAmmo(int32 Amount);
	void SetFireType(const EFireType NewFireType) { CurrentFireType = NewFireType; }

protected:
	virtual void ProcessFire();
	virtual void SpawnProjectile();

	virtual void StartReload();
	virtual void FinishReload();

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
	EFireType CurrentFireType = EFireType::Auto;
	bool bIsReloading = false;
	bool bCanFire = true;
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;
	FTimerHandle ReloadTimer;
	
	UPROPERTY()
	TObjectPtr<class UNiagaraSystem> MuzzleFlash;

	UPROPERTY()
	TObjectPtr<class USoundCue> FireSound;
};
