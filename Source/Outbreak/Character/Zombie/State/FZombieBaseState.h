// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Core/TStateMachine.h"

class OUTBREAK_API FZombieBaseState : public TState<EZombieStateType>
{
public:
	using Super = FZombieBaseState;
	
	FZombieBaseState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, const EZombieStateType InStateKey, ACharacterZombie* InOwner);
	virtual void Enter(EZombieStateType PreviousState) override;
	virtual void Execute(EZombieStateType CurrentState, float DeltaTime) override;
	virtual void Exit(EZombieStateType NextState) override;

protected:
	TObjectPtr<ACharacterPlayer> GetTarget() const;

	TObjectPtr<ACharacterZombie> Owner;
	TObjectPtr<AZombieAIComponent> AIController;
	float RotationSpeed = 180.0f;
};
