#include "FZombieIdleState.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Component/ZombieAIComponent.h"

FZombieIdleState::FZombieIdleState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm, ACharacterZombie* InOwner): FZombieBaseState(InFsm, EZombieStateType::Idle, InOwner) { }

void FZombieIdleState::Enter(const EZombieStateType PreviousState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	Super::Enter(PreviousState, TargetPlayer);
	
	Owner->GetController()->StopMovement();

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

void FZombieIdleState::Exit(const EZombieStateType NextState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	Super::Exit(NextState, TargetPlayer);
	
}