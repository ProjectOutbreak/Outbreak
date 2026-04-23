#include "FZombieDieState.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

FZombieDieState::FZombieDieState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacter* InOwner)
: FZombieBaseState(InFsm, EZombieStateType::Die, InOwner)
{
}

void FZombieDieState::Enter(const EZombieStateType PreviousState)
{
	Super::Enter(PreviousState);
}

void FZombieDieState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	Super::Execute(CurrentState, DeltaTime);
}

void FZombieDieState::Exit(const EZombieStateType NextState)
{
	Super::Exit(NextState);
}