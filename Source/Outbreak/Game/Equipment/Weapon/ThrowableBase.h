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
	virtual void OnEquip() override;
	virtual void OnUse() override;
	virtual void OnEndUse() override;
	virtual bool CanUse() const override;
	virtual bool IsActive() const override;

	UFUNCTION(BlueprintCallable)
	void Throw();

protected:
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_ThrowAnim(UAnimMontage* MontageToPlay);

// --------------------
// Variables
// --------------------
protected:
	FThrowableData ThrowableData;

	UPROPERTY()
	int32 CurrentAmmo = 3;

	bool bIsThrowing = false;
	
};
