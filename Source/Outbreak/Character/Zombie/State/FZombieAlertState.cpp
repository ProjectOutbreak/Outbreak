#include "FZombieAlertState.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

FZombieAlertState::FZombieAlertState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacterZombie* InOwner) : FZombieBaseState(InFsm, EZombieStateType::Alert, InOwner) { }

void FZombieAlertState::Enter(const EZombieStateType PreviousState)
{
	Super::Enter(PreviousState);

	if (!Owner->HasAuthority()) return;

	Owner->SetIsScreaming(true);
}

void FZombieAlertState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	Super::Execute(CurrentState, DeltaTime);

	Timer += DeltaTime;
	if (Timer >= AlertAnimationLength)
	{
		Fsm->ChangeState(EZombieStateType::Chase);
	}
}

void FZombieAlertState::Exit(const EZombieStateType NextState)
{
	Super::Exit(NextState);
	
	Timer = 0.0f;

	if (!Owner->HasAuthority()) return;

	Owner->SetIsScreaming(false);
}