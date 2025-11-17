#pragma once

#include "CoreMinimal.h"
#include "FZombieBaseState.h"

class OUTBREAK_API FZombieAlertState : public FZombieBaseState
{
public:
	FZombieAlertState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacterZombie* InOwner);

	virtual void Enter(EZombieStateType PreviousState) override;
	virtual void Execute(EZombieStateType CurrentState, float DeltaTime) override;
	virtual void Exit(EZombieStateType NextState) override;

private:
	// TODO : delete magic number
	float AlertAnimationLength = 1.6f;
	float Timer = 0.0f;
};
