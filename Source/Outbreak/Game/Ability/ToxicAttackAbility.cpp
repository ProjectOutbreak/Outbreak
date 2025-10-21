// Fill out your copyright notice in the Description page of Project Settings.


#include "ToxicAttackAbility.h"

UToxicAttackAbility::UToxicAttackAbility()
{
	AbilityType = EAbilityType::ToxicAttack;
}

void UToxicAttackAbility::OnEquip()
{
	ACharacterBase* OwnerCharacter = Cast<ACharacterBase>(GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->OnAttackHit.AddDynamic(this, &UToxicAttackAbility::HandleOwnerAttackHit);
	}
}

void UToxicAttackAbility::OnUnequip()
{
	ACharacterBase* OwnerCharacter = Cast<ACharacterBase>(GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->OnAttackHit.RemoveDynamic(this, &UToxicAttackAbility::HandleOwnerAttackHit);
	}
}

void UToxicAttackAbility::HandleOwnerAttackHit(AActor* HitActor, const FHitResult& HitResult)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(HitActor))
	{
		Character->ApplyToxicDamage(ToxicDamagePerSecond, ToxicDuration);
	}
}