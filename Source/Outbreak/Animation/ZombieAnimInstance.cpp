// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Public/Utilities/DebugHelper.h"

void UZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerZombie = Cast<ACharacterZombie>(TryGetPawnOwner());
	if (!OwnerZombie)
	{
		PRINT_WITH_CURRENT_CONTEXT("OwnerZombie is nullptr");
		return;
	}
	MovementComponent = OwnerZombie->GetCharacterMovement();
	if (!MovementComponent)
	{
		PRINT_WITH_CURRENT_CONTEXT("MovementComponent is nullptr");
		return;
	}
}

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!IsValid(OwnerZombie))
	{
		OwnerZombie = Cast<ACharacterZombie>(TryGetPawnOwner());

		if (!IsValid(OwnerZombie)) return;

		MovementComponent = OwnerZombie->GetCharacterMovement();
	}

	if (!IsValid(MovementComponent)) return;

	AttackRate = OwnerZombie->GetZombieData()->AttackRate;
	Velocity = MovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, OwnerZombie->GetActorRotation());
	IsAttack = OwnerZombie->GetIsAttacking();
	IsScream = OwnerZombie->GetIsScreaming();
	ShouldMove = GroundSpeed > 3.0f;
}
