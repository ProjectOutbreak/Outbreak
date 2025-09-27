// Fill out your copyright notice in the Description page of Project Settings.

#include "PopperZombie.h"
#include "Outbreak/Component/AbilityComponent.h"
#include "Outbreak/Game/Ability/SelfDestructAbility.h"

APopperZombie::APopperZombie()
{
	ZombieSubType = EZombieSubType::Popper;
	BodyScale = 0.7f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ZombieMesh(TEXT("/Game/Art/Characters/Zombies/Meshes/SKM_Zombie_Fat_002.SKM_Zombie_Fat_002"));
	if (ZombieMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(ZombieMesh.Object);
	}
}

void APopperZombie::BeginPlay()
{
	Super::BeginPlay();

	AbilityComponent->AddAbility(NewObject<USelfDestructAbility>(AbilityComponent));
}

void APopperZombie::OnRep_Die()
{
	Super::OnRep_Die();

	AbilityComponent->TryActivateAbility(EAbilityType::SelfDestruct);
}