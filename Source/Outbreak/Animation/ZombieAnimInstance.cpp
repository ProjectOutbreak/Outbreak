// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Utilities/DebugHelper.h"

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

	UpperBodyBlendAlpha = bIsAttacking ? 1.0f : 0.0f;
}

void UZombieAnimInstance::PlayAttackMontage()
{
	if (AttackMontages.Num() == 0) return;
	
	const int32 RandomIndex = FMath::RandRange(0, AttackMontages.Num() - 1);
	UAnimMontage* SelectedMontage = AttackMontages[RandomIndex];

	FOnMontageEnded OnAttackMontageEnded;
	OnAttackMontageEnded.BindUObject(this, &UZombieAnimInstance::OnAttackMontageEnded);
	
	Montage_Play(SelectedMontage);
	Montage_SetEndDelegate(OnAttackMontageEnded, SelectedMontage);
	bIsAttacking = true;
}

void UZombieAnimInstance::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;
}

void UZombieAnimInstance::PlayScreamingMontage(FOnMontageEnded& OnMontageEndedDelegate)
{
	if (!ScreamingMontage) return;

	Montage_Play(ScreamingMontage);
	Montage_SetEndDelegate(OnMontageEndedDelegate, ScreamingMontage);
}
