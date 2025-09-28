#include "FZombieDieState.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

FZombieDieState::FZombieDieState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm,
	ACharacterZombie* InOwner): FZombieBaseState(InFsm, EZombieStateType::Die, InOwner)
{
}

void FZombieDieState::Enter(const EZombieStateType PreviousState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	FZombieBaseState::Enter(PreviousState, TargetPlayer);
	// TODO : Score, Exp
}

void FZombieDieState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	FZombieBaseState::Execute(CurrentState, DeltaTime);
}

void FZombieDieState::Exit(const EZombieStateType NextState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	FZombieBaseState::Exit(NextState, TargetPlayer);
}