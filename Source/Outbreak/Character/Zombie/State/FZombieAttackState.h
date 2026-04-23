#pragma once

#include "CoreMinimal.h"
#include "FZombieBaseState.h"
#include "Outbreak/Util/Define.h"

class OUTBREAK_API FZombieAttackState : public FZombieBaseState
{
public:
	FZombieAttackState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacter* InOwner);

	virtual void Enter(EZombieStateType PreviousState) override;
	virtual void Execute(EZombieStateType CurrentState, float DeltaTime) override;
	virtual void Exit(EZombieStateType NextState) override;

protected:
	bool IsOutOfAttackRange();
};
