#include "FZombieAttackState.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

FZombieAttackState::FZombieAttackState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacter* InOwner)
	: FZombieBaseState(InFsm, EZombieStateType::Attack, InOwner)
{
}

void FZombieAttackState::Enter(const EZombieStateType PreviousState)
{
	Super::Enter(PreviousState);

	if (!OwnerCharacter->HasAuthority()) return;
	
	Owner->SetIsAttacking(true);
}

void FZombieAttackState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	Super::Execute(CurrentState, DeltaTime);
	
	if (IsOutOfAttackRange())
	{
		ChangeState(EZombieStateType::Chase);
	}
}

void FZombieAttackState::Exit(const EZombieStateType NextState)
{
	Super::Exit(NextState);
	
	Owner->SetIsAttacking(false);
}

bool FZombieAttackState::IsOutOfAttackRange()
{
	TObjectPtr<ACharacterPlayer> CurrentTarget = GetTarget();
	if (!CurrentTarget) return true;
	
	const FZombieData* ZombieData = Owner->GetZombieData();
	const float AttackRange = ZombieData->AttackRange;
	const float DistanceToTarget = FVector::Dist(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	
	if (DistanceToTarget > AttackRange) return true;
	
	return false;
}