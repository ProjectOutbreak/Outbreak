// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialZombie.h"
#include "ToxicZombie.generated.h"

UCLASS()
class OUTBREAK_API AToxicZombie : public ASpecialZombie
{
	GENERATED_BODY()

protected:
	virtual void InitCharacterData() override;
	virtual void OnRep_Die() override;

private:
	void AttachToxicEffect();
	void ReleaseToxicEffect();

private:
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> ToxicEffect;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> ToxicEffectComponent;
};
