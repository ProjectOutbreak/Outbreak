// Fill out your copyright notice in the Description page of Project Settings.

#include "BloaterZombie.h"

void ABloaterZombie::InitCharacterData()
{
	ZombieSubType = EZombieSubType::Bloater;
	CharacterBodyType = ECharacterBodyType::Fat;
	
	Super::InitCharacterData();
}