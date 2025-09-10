// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialZombie.h"
#include "GymRatZombie.generated.h"

UCLASS()
class OUTBREAK_API AGymRatZombie : public ASpecialZombie
{
	GENERATED_BODY()

public:
	AGymRatZombie();

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_Die() override;

private:
	void AttachVibrationEffect();
	void ReleaseVibrationEffect();

private:
	UPROPERTY()
	TObjectPtr<class UNiagaraSystem> VibrationEffect;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> VibrationEffectComponent;
};
