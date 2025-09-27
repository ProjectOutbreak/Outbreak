// Fill out your copyright notice in the Description page of Project Settings.

#include "WalkerZombie.h"
#include "Outbreak/Util/MeshLoadHelper.h"

AWalkerZombie::AWalkerZombie()
{
	ZombieSubType = EZombieSubType::Walker;
}

void AWalkerZombie::BeginPlay()
{
	SetMesh();
	
	Super::BeginPlay();
}

void AWalkerZombie::SetMesh(ECharacterBodyType MeshType)
{
	Super::SetMesh(MeshType);
}
