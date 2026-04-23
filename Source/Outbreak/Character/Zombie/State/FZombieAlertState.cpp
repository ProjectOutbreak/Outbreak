#include "FZombieAlertState.h"
#include "Animation/ZombieAnimInstance.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

FZombieAlertState::FZombieAlertState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacter* InOwner)
	: FZombieBaseState(InFsm, EZombieStateType::Alert, InOwner) { }

void FZombieAlertState::Enter(const EZombieStateType PreviousState)
{
	Super::Enter(PreviousState);

	if (!OwnerCharacter->HasAuthority()) return;
	
	if (UZombieAnimInstance* AnimInst = GetAnimInstance())
	{
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindRaw(this, &FZombieAlertState::OnAlertMontageEnded);
		
		AnimInst->PlayAlertMontage(MontageEndedDelegate);
	}
}

void FZombieAlertState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	Super::Execute(CurrentState, DeltaTime);
}

void FZombieAlertState::Exit(const EZombieStateType NextState)
{
	Super::Exit(NextState);
}

void FZombieAlertState::OnAlertMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Fsm->ChangeState(EZombieStateType::Chase);
}
