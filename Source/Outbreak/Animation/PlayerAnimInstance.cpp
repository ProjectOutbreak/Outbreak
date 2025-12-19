// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	OwnerCharacter = Cast<ACharacterPlayer>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		MovementComponent = OwnerCharacter->GetCharacterMovement();
		EquipmentController = OwnerCharacter->GetEquipmentController();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!OwnerCharacter.Get() || !MovementComponent.Get()) return;

	Velocity = MovementComponent->Velocity;
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, OwnerCharacter->GetActorRotation());
    
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
	bIsFalling = MovementComponent->IsFalling();
    
	const FVector CurrentAcceleration = MovementComponent->GetCurrentAcceleration();
	const bool bIsAccelerating = !CurrentAcceleration.IsNearlyZero();
    
	bShouldMove = (GroundSpeed > 3.0f) || bIsAccelerating;
	
	if (EquipmentController.IsValid())
	{
		bIsOnUse = EquipmentController->GetIsOnUse();
		bIsOnReload = EquipmentController->GetIsReload();
		bIsSwapIn = EquipmentController->GetIsSwapIn();
		bIsSwapOut = EquipmentController->GetIsSwapOut();
		CurrentAmmoInMag = EquipmentController->GetCurrentAmmoInMag();
		FireType = EquipmentController->GetCurrentFireType();
		if (const AEquipmentBase* EquippedItem = EquipmentController->GetCurrentEquippedItem())
		{
			CurrentEquipmentType = EquippedItem->GetEquipmentType();
		}
	}
}