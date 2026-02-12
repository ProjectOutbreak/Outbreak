// Fill out your copyright notice in the Description page of Project Settings.

#include "WalkerZombie.h"

AWalkerZombie::AWalkerZombie()
{
	ZombieSubType = EZombieSubType::Walker;
}

void AWalkerZombie::BeginPlay()
{
	SetMesh(ECharacterBodyType::Normal);
	
	Super::BeginPlay();
}