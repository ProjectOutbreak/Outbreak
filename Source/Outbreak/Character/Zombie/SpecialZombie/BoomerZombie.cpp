// Fill out your copyright notice in the Description page of Project Settings.

#include "BoomerZombie.h"
#include "Outbreak/Component/AbilityComponent.h"
#include "Outbreak/Game/Ability/SelfDestructAbility.h"

void ABoomerZombie::InitCharacterData()
{
	ZombieSubType = EZombieSubType::Boomer;
	CharacterBodyType = ECharacterBodyType::Fat;
	AbilityComponent->AddAbility(NewObject<USelfDestructAbility>(AbilityComponent));
	
	Super::InitCharacterData();
}

void ABoomerZombie::OnRep_Die()
{
	Super::OnRep_Die();
	
	AbilityComponent->TryActivateAbility(EAbilityType::SelfDestruct);
}