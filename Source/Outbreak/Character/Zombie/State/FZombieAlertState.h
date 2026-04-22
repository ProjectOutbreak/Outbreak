#pragma once

#include "CoreMinimal.h"
#include "FZombieBaseState.h"

class OUTBREAK_API FZombieAlertState : public FZombieBaseState
{
public:
	FZombieAlertState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacter* InOwner);

	virtual void Enter(EZombieStateType PreviousState) override;
	virtual void Execute(EZombieStateType CurrentState, float DeltaTime) override;
	virtual void Exit(EZombieStateType NextState) override;

private:
	void OnAlertMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
