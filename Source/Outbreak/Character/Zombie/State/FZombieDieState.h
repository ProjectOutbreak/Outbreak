#pragma once
#include "FZombieBaseState.h"

class FZombieDieState : public FZombieBaseState
{
public:
	FZombieDieState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacterZombie* InOwner);

	virtual void Enter(EZombieStateType PreviousState) override;
	virtual void Execute(EZombieStateType CurrentState, float DeltaTime) override;
	virtual void Exit(EZombieStateType NextState) override; 
};
