// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Core/TStateMachine.h"

class UZombieAnimInstance;

class OUTBREAK_API FZombieBaseState : public TState<EZombieStateType>
{
public:
	using Super = FZombieBaseState;
	
	FZombieBaseState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, const EZombieStateType InStateKey, ACharacter* InOwner);
	virtual void Enter(EZombieStateType PreviousState) override;
	virtual void Execute(EZombieStateType CurrentState, float DeltaTime) override;
	virtual void Exit(EZombieStateType NextState) override;

protected:
	TObjectPtr<UZombieAnimInstance> GetAnimInstance();

protected:
	TObjectPtr<ACharacterPlayer> GetTarget() const;

	TScriptInterface<IZombieInterface> Owner;
	ACharacter* OwnerCharacter;
	TObjectPtr<AZombieAIComponent> AIController;
	float RotationSpeed = 180.0f;

private:
	TObjectPtr<UZombieAnimInstance> CachedAnimInstance;
};
