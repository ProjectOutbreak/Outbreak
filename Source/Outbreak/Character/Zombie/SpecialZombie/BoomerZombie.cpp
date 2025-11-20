// Fill out your copyright notice in the Description page of Project Settings.

#include "BoomerZombie.h"
#include "Outbreak/Component/AbilityComponent.h"
#include "Outbreak/Game/Ability/SelfDestructAbility.h"

ABoomerZombie::ABoomerZombie()
{
	ZombieSubType = EZombieSubType::Boomer;
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ZombieMesh(TEXT("/Game/Art/Characters/Zombies/Meshes/SKM_Zombie_Fat_002.SKM_Zombie_Fat_002"));
	if (ZombieMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(ZombieMesh.Object);
	}
}

void ABoomerZombie::BeginPlay()
{
	Super::BeginPlay();
	
	AbilityComponent->AddAbility(NewObject<USelfDestructAbility>(AbilityComponent));
}

void ABoomerZombie::OnRep_Die()
{
	Super::OnRep_Die();
	
	AbilityComponent->TryActivateAbility(EAbilityType::SelfDestruct);
}