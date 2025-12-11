#include "FZombieAlertState.h"
#include "Outbreak/Animation/ZombieAnimInstance.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

FZombieAlertState::FZombieAlertState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacterZombie* InOwner) : FZombieBaseState(InFsm, EZombieStateType::Alert, InOwner) { }

void FZombieAlertState::Enter(const EZombieStateType PreviousState)
{
	Super::Enter(PreviousState);

	if (!Owner->HasAuthority()) return;

	UZombieAnimInstance* AnimInst = GetAnimInstance();
	if (!AnimInst) return;

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindRaw(this, &FZombieAlertState::OnScreamingMontageEnded);
	
	AnimInst->PlayScreamingMontage(MontageEndedDelegate);
}

void FZombieAlertState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	Super::Execute(CurrentState, DeltaTime);
}

void FZombieAlertState::Exit(const EZombieStateType NextState)
{
	Super::Exit(NextState);
	
	if (!Owner->HasAuthority()) return;

	Owner->StopAnimMontage();
}

void FZombieAlertState::OnScreamingMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Fsm->ChangeState(EZombieStateType::Chase);
}
