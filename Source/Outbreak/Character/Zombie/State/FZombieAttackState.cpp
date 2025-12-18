#include "FZombieAttackState.h"
#include "Outbreak/Animation/ZombieAnimInstance.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

FZombieAttackState::FZombieAttackState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacterZombie* InOwner): FZombieBaseState(InFsm, EZombieStateType::Attack, InOwner) { }

void FZombieAttackState::Enter(const EZombieStateType PreviousState)
{
	Super::Enter(PreviousState);

	if (!Owner->HasAuthority()) return;

	UZombieAnimInstance* AnimInst = GetAnimInstance();
	if (!AnimInst) return;

	AnimInst->PlayAttackMontage();
}

void FZombieAttackState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	Super::Execute(CurrentState, DeltaTime);
	
	if (IsOutOfAttackRange())
	{
		Owner->StopAnimMontage();
		ChangeState(EZombieStateType::Alert);
	}
}

void FZombieAttackState::Exit(const EZombieStateType NextState)
{
	Super::Exit(NextState);

	if (!Owner->HasAuthority()) return;

	Owner->StopAnimMontage();
}

bool FZombieAttackState::IsOutOfAttackRange()
{
	TObjectPtr<ACharacterPlayer> CurrentTarget = GetTarget();
	if (!CurrentTarget) return true;
	
	const FZombieData* ZombieData = Owner->GetZombieData();
	const float AttackRange = ZombieData->AttackRange;
	const float DistanceToTarget = FVector::Dist(Owner->GetActorLocation(), CurrentTarget->GetActorLocation());
	
	if (DistanceToTarget > AttackRange) return true;
	
	return false;
}