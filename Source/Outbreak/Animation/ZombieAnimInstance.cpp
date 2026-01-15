// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

void UZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerZombie = Cast<ACharacterZombie>(TryGetPawnOwner());
	if (!OwnerZombie) return;
	MovementComponent = OwnerZombie->GetCharacterMovement();
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
	
	Velocity = MovementComponent->Velocity;
	
	const float RawGroundSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
	const float RawDirection = UKismetAnimationLibrary::CalculateDirection(Velocity, OwnerZombie->GetActorRotation());

	CurrentGroundSpeed = FMath::FInterpTo(CurrentGroundSpeed, RawGroundSpeed, DeltaSeconds, GroundSpeedInterpSpeed);
	CurrentDirection = FMath::FInterpTo(CurrentDirection, RawDirection, DeltaSeconds, DirectionInterpSpeed);
	GroundSpeed = CurrentGroundSpeed;
	Direction = CurrentDirection;
	
	AttackRate = OwnerZombie->GetZombieData()->AttackRate;
	ShouldMove = GroundSpeed > 3.0f;
	
	bIsAttacking = OwnerZombie->IsAttacking();

	UpperBodyBlendAlpha = bIsAttacking ? 1.0f : 0.0f;
}

void UZombieAnimInstance::PlayAlertMontage(FOnMontageEnded OnMontageEndedDelegate)
{
	if (!ScreamingMontage) return;

	Montage_Play(ScreamingMontage);
	if (OnMontageEndedDelegate.IsBound())
	{
		Montage_SetEndDelegate(OnMontageEndedDelegate, ScreamingMontage);
	}
}