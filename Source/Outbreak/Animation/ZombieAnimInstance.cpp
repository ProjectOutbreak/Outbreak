// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

void UZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerZombie = Cast<ACharacterZombie>(TryGetPawnOwner());
	if (!OwnerZombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] OwnerZombie is nullptr"), CURRENT_CONTEXT);
		return;
	}
	MovementComponent = OwnerZombie->GetCharacterMovement();
	if (!MovementComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] MovementComponent is nullptr"), CURRENT_CONTEXT);
		return;
	}
}

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!OwnerZombie || !MovementComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] OwnerZombie or MovementComponent is nullptr"), CURRENT_CONTEXT);
		return;
	}

	AttackRate = OwnerZombie->GetZombieData()->AttackRate;
	Velocity = MovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, OwnerZombie->GetActorRotation());
	IsAttack = OwnerZombie->bIsAttacking;
	IsScream = OwnerZombie->bIsScreaming;
	ShouldMove = GroundSpeed > 3.0f;
}
