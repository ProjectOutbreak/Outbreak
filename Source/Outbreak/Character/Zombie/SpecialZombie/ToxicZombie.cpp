// Fill out your copyright notice in the Description page of Project Settings.

#include "ToxicZombie.h"
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
}

void AToxicZombie::BeginPlay()
{
	SetPhysicalAsset(ECharacterType::Zombie, ECharacterBodyType::Normal);
	
	Super::BeginPlay();
	SetActorScale3D(FVector(BodyScale, BodyScale, BodyScale));

	AbilityComponent->AddAbility(NewObject<UToxicAuraAbility>(AbilityComponent));
	AbilityComponent->AddAbility(NewObject<UToxicAttackAbility>(AbilityComponent));
}