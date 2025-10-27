// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Core/TStateMachine.h"

class OUTBREAK_API FZombieBaseState : public TState<EZombieStateType, ACharacterPlayer>
{
public:
	using Super = FZombieBaseState;
	
	FZombieBaseState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm, const EZombieStateType InStateKey, ACharacterZombie* InOwner);
	virtual void Enter(EZombieStateType PreviousState, TObjectPtr<ACharacterPlayer> TargetPlayer) override;
	virtual void Execute(EZombieStateType CurrentState, float DeltaTime) override;
	virtual void Exit(EZombieStateType NextState, TObjectPtr<ACharacterPlayer> TargetPlayer) override;

protected:
	void RotateTowardsTarget(float DeltaTime) const;


protected:
	TObjectPtr<ACharacterZombie> Owner;
	TObjectPtr<ACharacterPlayer> CurrentTargetPlayer;
	float RotationSpeed = 180.0f;
};
