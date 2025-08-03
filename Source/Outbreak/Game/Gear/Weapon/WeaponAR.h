// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainWeapon.h"
#include "Outbreak/Data/GameData.h"
#include "WeaponAR.generated.h"

UCLASS()
class OUTBREAK_API AWeaponAR : public AMainWeapon
{
	GENERATED_BODY()
public:
	AWeaponAR();
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void Reload() override;
	virtual void InitializeWeaponData(FWeaponData* InData) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void NotifyAmmoUpdate() override;

	UFUNCTION(Server,Reliable)
	void ServerStartFire();

	UFUNCTION(Server, Reliable)
	void ServerStopFire();
	
	UFUNCTION(NetMulticast,Unreliable)
	void MultiCastShot();

	UFUNCTION(Server, Reliable)
	void ServerReload();
	
	void MakeShot();
	void FinishReload();
	void PlayMuzzleEffect();

protected:
	UPROPERTY(Replicated)
	FWeaponData WeaponData;
	
	FTimerHandle TimerHandle_TimeBetweenShots;
	FTimerHandle ReloadTimerHandle;
};
