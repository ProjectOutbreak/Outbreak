#pragma once
#include "FZombieBaseState.h"
#include "Outbreak/Util/Define.h"

enum class EZombieStateType : uint8;

class FZombieWanderState : public FZombieBaseState
{
public:
	FZombieWanderState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacter* InOwner);

	virtual void Enter(EZombieStateType PreviousState) override;
	virtual void Execute(EZombieStateType CurrentState, float DeltaTime) override;
	virtual void Exit(EZombieStateType NextState) override;

private:
	float WanderRadius = 500.0f;
	float WanderSpeed = 100.0f;
	float MinWanderTime = 5.0f;
	float MaxWanderTime = 15.0f;

	float WanderTimer = 0.0f;

	void StartWandering();
	bool FindRandomWanderLocation(FVector& OutLocation);
};
