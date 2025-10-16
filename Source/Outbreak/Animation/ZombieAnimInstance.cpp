// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieAnimInstance.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

void UZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerZombie = Cast<ACharacterZombie>(TryGetPawnOwner());
}

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!OwnerZombie) return;

	AttackRate = OwnerZombie->GetZombieData()->AttackRate;
}
