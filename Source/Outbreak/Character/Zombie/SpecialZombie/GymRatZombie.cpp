// Fill out your copyright notice in the Description page of Project Settings.

#include "GymRatZombie.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Outbreak/Component/AbilityComponent.h"
#include "Outbreak/Game/Ability/VibrationAbility.h"

AGymRatZombie::AGymRatZombie()
{
	ZombieSubType = EZombieSubType::GymRat;
	BodyScale = 1.5f;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FootstepEffectAsset(TEXT("/Game/Art/VFX/Niagara/NS_Smoke_A.NS_Smoke_A"));
	if (FootstepEffectAsset.Succeeded())
	{
		VibrationEffect = FootstepEffectAsset.Object;
	}
}

void AGymRatZombie::BeginPlay()
{
	SetMesh(ECharacterBodyType::Muscle);
	SetPhysicalAsset(ECharacterType::Zombie, ECharacterBodyType::Muscle);
	
	Super::BeginPlay();

	SetActorScale3D(FVector(BodyScale, BodyScale, BodyScale));
	
	AbilityComponent->AddAbility(NewObject<UVibrationAbility>(AbilityComponent));
	AttachVibrationEffect();
}

void AGymRatZombie::OnRep_Die()
{
	Super::OnRep_Die();

	ReleaseVibrationEffect();
}

void AGymRatZombie::AttachVibrationEffect()
{
	if (!VibrationEffect)
		return;
	
	USkeletalMeshComponent* OwnerMesh = GetMesh();
	if (OwnerMesh)
	{
		VibrationEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(VibrationEffect, OwnerMesh, TEXT("ik_foot_root"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	}
}

void AGymRatZombie::ReleaseVibrationEffect()
{
	if (!VibrationEffectComponent)
		return;

	VibrationEffectComponent->DestroyComponent();
	VibrationEffectComponent = nullptr;
}