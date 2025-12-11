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

	AttackRate = OwnerZombie->GetZombieData()->AttackRate;
	Velocity = MovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, OwnerZombie->GetActorRotation());
	ShouldMove = GroundSpeed > 3.0f;
}

void UZombieAnimInstance::PlayAttackMontage()
{
	if (AttackMontages.Num() == 0) return;

	const int32 RandomIndex = FMath::RandRange(0, AttackMontages.Num() - 1);
	UAnimMontage* SelectedMontage = AttackMontages[RandomIndex];

	Montage_Play(SelectedMontage);
}

void UZombieAnimInstance::PlayScreamingMontage(FOnMontageEnded& OnMontageEndedDelegate)
{
	if (!ScreamingMontage) return;

	Montage_Play(ScreamingMontage);
	Montage_SetEndDelegate(OnMontageEndedDelegate, ScreamingMontage);
}
