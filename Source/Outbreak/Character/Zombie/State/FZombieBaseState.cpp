// Fill out your copyright notice in the Description page of Project Settings.

#include "FZombieBaseState.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Component/ZombieAIComponent.h"
#include "Outbreak/Util/Define.h"

FZombieBaseState::FZombieBaseState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, const EZombieStateType InStateKey, ACharacterZombie* InOwner): TState(InFsm, InStateKey)
{
	Owner = InOwner;
	AIController = Cast<AZombieAIComponent>(InOwner->GetController());
}

void FZombieBaseState::Enter(EZombieStateType PreviousState) { }

void FZombieBaseState::Execute(EZombieStateType CurrentState, float DeltaTime) { }

void FZombieBaseState::Exit(EZombieStateType NextState) { }

void FZombieBaseState::RotateTowardsTarget(const float DeltaTime) const
{
	const TObjectPtr<ACharacterPlayer> CurrentTarget = GetTarget();
	
	if (Owner && CurrentTarget)
	{
		const FVector TargetLocation = CurrentTarget->GetActorLocation();
		const FVector OwnerLocation = Owner->GetActorLocation();
        
		FVector Direction = TargetLocation - OwnerLocation;
		Direction.Z = 0.0f;
		Direction.Normalize();
        
		if (!Direction.IsNearlyZero())
		{
			const FRotator TargetRotation = Direction.Rotation();
			const FRotator CurrentRotation = Owner->GetActorRotation();
            
			const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
            
			Owner->SetActorRotation(NewRotation);
		}
	}
}

TObjectPtr<ACharacterPlayer> FZombieBaseState::GetTarget() const
{
	if (AIController)
	{
		return AIController->GetTarget(); 
	}
	return nullptr;
}