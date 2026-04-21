// Fill out your copyright notice in the Description page of Project Settings.

#include "WalkerZombie.h"

void AWalkerZombie::InitCharacterData()
{
	ZombieSubType = EZombieSubType::Walker;
	CharacterBodyType = ECharacterBodyType::Normal;
	
	Super::InitCharacterData();
}