// Fill out your copyright notice in the Description page of Project Settings.

#include "ToxicZombie.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Outbreak/Component/AbilityComponent.h"
#include "Outbreak/Game/Ability/ToxicAttackAbility.h"
#include "Outbreak/Game/Ability/ToxicAuraAbility.h"

AToxicZombie::AToxicZombie()
{
	ZombieSubType = EZombieSubType::Toxic;
	BodyScale = 1.3f;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ToxicMesh(TEXT("/Game/Art/Characters/Zombies/Meshes/SKM_Zombie_Toxic.SKM_Zombie_Toxic"));
	if (ToxicMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(ToxicMesh.Object);
	}
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ToxicEffectAsset(TEXT("/Game/Art/VFX/Niagara/NS_Smoke_D.NS_Smoke_D"));
	if (ToxicEffectAsset.Succeeded())
	{
		ToxicEffect = ToxicEffectAsset.Object;
	}
}

void AToxicZombie::BeginPlay()
{
	SetPhysicalAsset(ECharacterType::Zombie, ECharacterBodyType::Normal);
	
	Super::BeginPlay();

	AbilityComponent->AddAbility(NewObject<UToxicAuraAbility>(AbilityComponent));
	AbilityComponent->AddAbility(NewObject<UToxicAttackAbility>(AbilityComponent));
	AttachToxicEffect();
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