// Fill out your copyright notice in the Description page of Project Settings.

#include "PopperZombie.h"
#include "Outbreak/Component/AbilityComponent.h"
#include "Outbreak/Game/Ability/SelfDestructAbility.h"

void APopperZombie::InitCharacterData()
{
	ZombieSubType = EZombieSubType::Popper;
	CharacterBodyType = ECharacterBodyType::Fat;
	AbilityComponent->AddAbility(NewObject<USelfDestructAbility>(AbilityComponent));
	
	Super::InitCharacterData();
}

void APopperZombie::OnRep_Die()
{
	Super::OnRep_Die();

	AbilityComponent->TryActivateAbility(EAbilityType::SelfDestruct);
}