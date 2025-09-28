#pragma once

#include "CoreMinimal.h"
#include "FZombieBaseState.h"

class OUTBREAK_API FZombieAlertState : public FZombieBaseState
{
public:
	FZombieAlertState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm, ACharacterZombie* InOwner);

	virtual void Enter(EZombieStateType PreviousState, TObjectPtr<ACharacterPlayer> TargetPlayer = nullptr) override;
	virtual void Execute(EZombieStateType CurrentState, float DeltaTime) override;
	virtual void Exit(EZombieStateType NextState, TObjectPtr<ACharacterPlayer> TargetPlayer = nullptr) override;

private:
	float Timer = 0.0f;
};
