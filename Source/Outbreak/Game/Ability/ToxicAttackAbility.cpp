// Fill out your copyright notice in the Description page of Project Settings.

#include "ToxicAttackAbility.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"

UToxicAttackAbility::UToxicAttackAbility()
{
	AbilityType = EAbilityType::ToxicAttack;
}

void UToxicAttackAbility::OnEquip()
{
	if (ACharacterBase* OwnerCharacter = Cast<ACharacterBase>(GetOwner()))
	{
		OwnerCharacter->OnAttackOtherCharacter.AddDynamic(this, &UToxicAttackAbility::HandleAttackOtherCharacter);
	}
}

void UToxicAttackAbility::OnUnequip()
{
	if (ACharacterBase* OwnerCharacter = Cast<ACharacterBase>(GetOwner()))
	{
		OwnerCharacter->OnAttackOtherCharacter.RemoveDynamic(this, &UToxicAttackAbility::HandleAttackOtherCharacter);
	}
}

void UToxicAttackAbility::HandleAttackOtherCharacter(AActor* HitActor)
{
	if (ACharacterPlayer* Character = Cast<ACharacterPlayer>(HitActor))
	{
		Character->ApplyToxicDamage(ToxicDamagePerSecond, ToxicDuration);
	}
}