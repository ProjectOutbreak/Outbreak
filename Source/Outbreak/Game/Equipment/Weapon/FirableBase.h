// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "FirableBase.generated.h"

DECLARE_DELEGATE(FOnReloadFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmoInMag, int32, CurrentTotalAmmo);

UCLASS(Abstract)
class OUTBREAK_API AFirableBase : public AWeaponBase
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------	
public:
	AFirableBase();
	virtual void OnEquip() override { OnAmmoChanged.Broadcast(CurrentAmmoInMag, CurrentTotalAmmo); }
	virtual void OnUse() override { StartFire(); }
	virtual void OnEndUse() override { StopFire(); }
	virtual bool CanUse() const override { return !bIsInUse && !bIsReloading && CurrentAmmoInMag > 0; }
	virtual void StartReload(const FOnReloadFinished& DoneCallback);

	bool CanReload() const { return !bIsReloading && CurrentAmmoInMag < FirableData.MagazineSize && CurrentTotalAmmo > 0; }
	bool IsReloading() const { return bIsReloading; }
	void SetIsReloading(const bool bInIsReloading) { bIsReloading = bInIsReloading; }
	int32 GetCurrentAmmoInMag() const { return CurrentAmmoInMag; }
	int32 GetCurrentTotalAmmo() const { return CurrentTotalAmmo; }
	FFirableData GetFirableData() const { return FirableData; }
	EFireType GetCurrentFireType() const { return CurrentFireType; }
	void SetFireType(const EFireType NewFireType) { CurrentFireType = NewFireType; }
	EFireType ToggleFireMode();

protected:
	void StartFire();
	void StopFire();
	
	virtual void ProcessFire();
	virtual void FinishReload();
	
// --------------------
// Variables
// --------------------
public:
	UPROPERTY()
	FOnAmmoChanged OnAmmoChanged;
	
protected:
	UPROPERTY()
	TObjectPtr<UAnimSequence> FireAnim;

	UPROPERTY()
	TObjectPtr<UAnimSequence> ReloadAnim;
	
	UPROPERTY()
	TObjectPtr<UAnimSequence> ReloadEmptyAnim;
	
	FFirableData FirableData;
	EFirableType FirableType;
	int32 CurrentAmmoInMag = 30;
	int32 CurrentTotalAmmo = 1000;
	EFireType CurrentFireType = EFireType::Auto;
	bool bIsReloading = false;
	FTimerHandle FireTimerHandle;
	
	FOnReloadFinished OnReloadFinishedCallback;
};
