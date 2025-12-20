// Fill out your copyright notice in the Description page of Project Settings.

#include "GymRatZombie.h"
#include "Outbreak/Component/AbilityComponent.h"
#include "Outbreak/Game/Ability/VibrationAbility.h"

AGymRatZombie::AGymRatZombie()
{
	ZombieSubType = EZombieSubType::GymRat;
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ZombieMesh(TEXT("/Game/Art/Characters/Zombies/Meshes/SKM_Zombie_Muscle_003.SKM_Zombie_Muscle_003"));
	if (ZombieMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(ZombieMesh.Object);
	}
}

void AGymRatZombie::BeginPlay()
{
	Super::BeginPlay();
	
	AbilityComponent->AddAbility(NewObject<UVibrationAbility>(AbilityComponent));
}

void AGymRatZombie::OnRep_Die()
{
	Super::OnRep_Die();
	
	AbilityComponent->Release();
}