// Fill out your copyright notice in the Description page of Project Settings.

#include "GymRatZombie.h"
#include "Outbreak/Component/AbilityComponent.h"
#include "Outbreak/Game/Ability/VibrationAbility.h"

void AGymRatZombie::InitCharacterData()
{
	ZombieSubType = EZombieSubType::GymRat;
	CharacterBodyType = ECharacterBodyType::Muscle;
	AbilityComponent->AddAbility(NewObject<UVibrationAbility>(AbilityComponent));
	
	Super::InitCharacterData();
}

void AGymRatZombie::OnRep_Die()
{
	Super::OnRep_Die();
	
	AbilityComponent->Release();
}