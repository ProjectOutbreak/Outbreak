#include "FZombieAttackState.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

FZombieAttackState::FZombieAttackState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm, ACharacterZombie* InOwner): FZombieBaseState(InFsm, EZombieStateType::Attack, InOwner) { }

void FZombieAttackState::Enter(const EZombieStateType PreviousState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	FZombieBaseState::Enter(PreviousState, TargetPlayer);

	if (!Owner->HasAuthority()) return;

	Owner->SetIsAttacking(true);
}

void FZombieAttackState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	FZombieBaseState::Execute(CurrentState, DeltaTime);
	
	RotateTowardsTarget(DeltaTime);

	if (IsOutOfAttackRange())
	{
		ChangeState(EZombieStateType::Alert, CurrentTargetPlayer);
	}
}

void FZombieAttackState::Exit(const EZombieStateType NextState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	FZombieBaseState::Exit(NextState, TargetPlayer);

	if (!Owner->HasAuthority()) return;

	Owner->SetIsAttacking(false);
}

bool FZombieAttackState::IsOutOfAttackRange()
{
	if (!CurrentTargetPlayer) return true;
	
	const FZombieData* ZombieData = Owner->GetZombieData();
	const float AttackRange = ZombieData->AttackRange;
	const float DistanceToTarget = FVector::Dist(Owner->GetActorLocation(), CurrentTargetPlayer->GetActorLocation());
	
	if (DistanceToTarget > AttackRange) return true;
	
	return false;
}