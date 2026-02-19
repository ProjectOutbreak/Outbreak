// Fill out your copyright notice in the Description page of Project Settings.

#include "SpecialZombie.h"
#include "Outbreak/Component/AbilityComponent.h"

ASpecialZombie::ASpecialZombie()
{
	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));
}

void ASpecialZombie::InitCharacterData()
{
	ZombieType = EZombieType::Special;
	
	Super::InitCharacterData();
}

void ASpecialZombie::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	AbilityComponent->Release();
}