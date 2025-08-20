// Fill out your copyright notice in the Description page of Project Settings.


#include "MedicineBase.h"

bool AMedicineBase::CanUse() const
{
	return Super::CanUse();
}

void AMedicineBase::OnUse()
{
	Super::OnUse();
}

void AMedicineBase::UseSelf()
{
}

void AMedicineBase::UseOnTarget(TObjectPtr<class ACharacterPlayer> TargetCharacter)
{
}

void AMedicineBase::OnUseComplete()
{
}

void AMedicineBase::ApplyHealEffect(TObjectPtr<class ACharacterPlayer> TargetCharacter)
{
}
