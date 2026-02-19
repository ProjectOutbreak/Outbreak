// Fill out your copyright notice in the Description page of Project Settings.

#include "ToxicZombie.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Outbreak/Component/AbilityComponent.h"
#include "Outbreak/Game/Ability/ToxicAttackAbility.h"
#include "Outbreak/Game/Ability/ToxicAuraAbility.h"

void AToxicZombie::InitCharacterData()
{
	ZombieSubType = EZombieSubType::Toxic;
	CharacterBodyType = ECharacterBodyType::None;
	AbilityComponent->AddAbility(NewObject<UToxicAuraAbility>(AbilityComponent));
	AbilityComponent->AddAbility(NewObject<UToxicAttackAbility>(AbilityComponent));
	AttachToxicEffect();
	
	Super::InitCharacterData();
}

void AToxicZombie::OnRep_Die()
{
	Super::OnRep_Die();

	ReleaseToxicEffect();
}

void AToxicZombie::AttachToxicEffect()
{
	if (!ToxicEffect)
		return;

	USkeletalMeshComponent* OwnerMesh = GetMesh();
	if (OwnerMesh)
	{
		ToxicEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(ToxicEffect, OwnerMesh, TEXT("ik_foot_root"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	}
}

void AToxicZombie::ReleaseToxicEffect()
{
	if (!ToxicEffectComponent)
		return;

	ToxicEffectComponent->DestroyComponent();
	ToxicEffectComponent = nullptr;

	UBaseAbility* Ability = AbilityComponent->GetAbility(EAbilityType::ToxicAura);
	if (!Ability) return;

	if (UToxicAuraAbility* ToxicAbility = Cast<UToxicAuraAbility>(Ability))
	{
		ToxicAbility->OnUnequip();
	}
}