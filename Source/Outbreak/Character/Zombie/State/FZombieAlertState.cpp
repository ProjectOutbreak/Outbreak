#include "FZombieAlertState.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

FZombieAlertState::FZombieAlertState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm, ACharacterZombie* InOwner) : FZombieBaseState(InFsm, EZombieStateType::Alert, InOwner) { }

void FZombieAlertState::Enter(const EZombieStateType PreviousState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	FZombieBaseState::Enter(PreviousState, TargetPlayer);

	if (!Owner->HasAuthority()) return;

	Owner->SetIsScreaming(true);
}

void FZombieAlertState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	FZombieBaseState::Execute(CurrentState, DeltaTime);

	RotateTowardsTarget(DeltaTime);
	
	Timer += DeltaTime;
	if (Timer >= AlertAnimationLength)
	{
		Fsm->ChangeState(EZombieStateType::Chase, CurrentTargetPlayer);
	}
}

void FZombieAlertState::Exit(const EZombieStateType NextState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	FZombieBaseState::Exit(NextState, TargetPlayer);
	
	Timer = 0.0f;

	if (!Owner->HasAuthority()) return;

	Owner->SetIsScreaming(false);
}