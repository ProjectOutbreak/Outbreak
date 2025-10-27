// Fill out your copyright notice in the Description page of Project Settings.

#include "RunnerZombie.h"

ARunnerZombie::ARunnerZombie()
{
	ZombieSubType = EZombieSubType::Runner;
}

void ARunnerZombie::BeginPlay()
{
	SetMesh();
	
	Super::BeginPlay();
}

void ARunnerZombie::SetMesh(ECharacterBodyType MeshType)
{
	Super::SetMesh(MeshType);
}
