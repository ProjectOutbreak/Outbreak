// Fill out your copyright notice in the Description page of Project Settings.

#include "FZombieBaseState.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Util/Define.h"

FZombieBaseState::FZombieBaseState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm,
	const EZombieStateType InStateKey, ACharacterZombie* InOwner): TState(InFsm, InStateKey)
{
	Owner = InOwner;
}

void FZombieBaseState::Enter(EZombieStateType PreviousState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	if (Owner->HasAuthority())
	{
		// Owner->Multicast_PlayAnimation(StateKey);
	}
	if (TargetPlayer)
	{
		CurrentTargetPlayer = TargetPlayer;
	}
}

void FZombieBaseState::Execute(EZombieStateType CurrentState, float DeltaTime)
{
}

void FZombieBaseState::Exit(EZombieStateType NextState, TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	CurrentTargetPlayer = nullptr;
	CurrentAnimationLength = 0.0f;
}

void FZombieBaseState::RotateTowardsTarget(const float DeltaTime) const
{
	if (Owner && CurrentTargetPlayer)
	{
		const FVector TargetLocation = CurrentTargetPlayer->GetActorLocation();
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