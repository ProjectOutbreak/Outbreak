// Fill out your copyright notice in the Description page of Project Settings.


#include "BloaterZombie.h"

ABloaterZombie::ABloaterZombie()
{
	ZombieSubType = EZombieSubType::Bloater;
}

void ABloaterZombie::BeginPlay()
{
	SetMesh(ECharacterBodyType::Fat);
	SetPhysicalAsset(ECharacterType::Zombie, ECharacterBodyType::Fat);
	
	Super::BeginPlay();
}