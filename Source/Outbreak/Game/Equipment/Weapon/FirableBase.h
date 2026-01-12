// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Outbreak/Game/Framework/DefaultPlayerState.h"
#include "FirableBase.generated.h"

DECLARE_DELEGATE(FOnReloadFinished);

UCLASS(Abstract)
class OUTBREAK_API AFirableBase : public AWeaponBase
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------	
public:
	AFirableBase();
	virtual void OnEquip() override { }
	virtual void OnUse() override { StartFire(); }
	virtual void OnEndUse() override { StopFire(); }
	virtual bool CanUse() const override { return !bIsInUse && !bIsReloading && CurrentAmmoInMag > 0; }
	virtual bool IsActive() const override;
	virtual void StartReload(const FOnReloadFinished& DoneCallback);

	bool CanReload() const;
	int32 GetCurrentAmmoInMag() const { return CurrentAmmoInMag; }
	FFirableData GetFirableData() const { return FirableData; }
	EFireType GetCurrentFireType() const { return CurrentFireType; }
	EFireType ToggleFireMode();

protected:
	virtual void Tick(float DeltaSeconds) override;
	void StartFire();
	void StopFire();
	
	virtual void ProcessFire();
	virtual void FinishReload();

	void ApplyDamageToTarget(AController* InstigatorController, const FHitResult& HitResult);

private:
	UFUNCTION(Server, Reliable)
	void Server_ProcessHit(const FHitResult& HitResult);
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayFireEffects(const FVector MuzzleLocation, const FHitResult& HitResult);
	UFUNCTION(Client, Reliable)
	void Client_ApplyRecoil();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayReloadAnim(bool bIsReloadEmpty);

	void RecoverRecoil(float DeltaTime);
	void ApplyRecoilLogic();
	int32 GetReservedAmmo() const;
	
// --------------------
// Variables
// --------------------
public:
	FOnPlayerAmmoChangedSignature OnPlayerAmmoChangedDelegate;
	
protected:
	UPROPERTY()
	TObjectPtr<UAnimSequence> FireAnim;

	UPROPERTY()
	TObjectPtr<UAnimSequence> ReloadAnim;
	
	UPROPERTY()
	TObjectPtr<UAnimSequence> ReloadEmptyAnim;
	
	UPROPERTY()
	TObjectPtr<USoundBase> FireSound;
	
	FFirableData FirableData;
	int32 CurrentAmmoInMag = 30;
	EFireType CurrentFireType = EFireType::Auto;
	bool bIsReloading = false;
	bool bIsFiring = false;
	FTimerHandle FireTimerHandle;
	
	FOnReloadFinished OnReloadFinishedCallback;

	float RecoilRecoveryTime = 0.5f;
	FTimerHandle RecoilResetTimer;
	bool bIsRecoiling = false;
};
