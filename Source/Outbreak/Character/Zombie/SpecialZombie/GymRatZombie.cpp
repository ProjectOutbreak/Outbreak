// Fill out your copyright notice in the Description page of Project Settings.


#include "GymRatZombie.h"
#include "Outbreak/Component/AbilityComponent.h"
#include "Outbreak/Game/Ability/VibrationAbility.h"

AGymRatZombie::AGymRatZombie()
{
	ZombieSubType = EZombieSubType::GymRat;
	BodyScale = 1.5f;
}

void AGymRatZombie::BeginPlay()
{
	SetMesh(ECharacterBodyType::Muscle);
	SetPhysicalAsset(ECharacterType::Zombie, ECharacterBodyType::Muscle);
	
	Super::BeginPlay();

	SetActorScale3D(FVector(BodyScale, BodyScale, BodyScale));
	
	AbilityComponent->AddAbility(NewObject<UVibrationAbility>(AbilityComponent));
}