#include "FZombieIdleState.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Component/ZombieAIComponent.h"

FZombieIdleState::FZombieIdleState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacter* InOwner)
: FZombieBaseState(InFsm, EZombieStateType::Idle, InOwner)
{
	
}

void FZombieIdleState::Enter(const EZombieStateType PreviousState)
{
	Super::Enter(PreviousState);
	
	OwnerCharacter->GetController()->StopMovement();

	IdleTime = FMath::RandRange(MinIdleTime, MaxIdleTime);
	IdleTimer = 0.0f;
}

void FZombieIdleState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	Super::Execute(CurrentState, DeltaTime);
	
	IdleTimer += DeltaTime;

	if (IdleTimer >= IdleTime)
	{
		Fsm->ChangeState(EZombieStateType::Wander);
	}
}

void FZombieIdleState::Exit(const EZombieStateType NextState)
{
	Super::Exit(NextState);
	
}