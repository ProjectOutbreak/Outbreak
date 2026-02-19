// Fill out your copyright notice in the Description page of Project Settings.

#include "RunnerZombie.h"

void ARunnerZombie::InitCharacterData()
{
	ZombieSubType = EZombieSubType::Runner;
	CharacterBodyType = ECharacterBodyType::Normal;
	
	Super::InitCharacterData();
}