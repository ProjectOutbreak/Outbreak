#include "FZombieDieState.h"

#include "Components/CapsuleComponent.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

FZombieDieState::FZombieDieState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm,
	ACharacterZombie* InOwner): FZombieBaseState(InFsm, EZombieStateType::Die, InOwner)
{
}

void FZombieDieState::Enter(const EZombieStateType PreviousState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	FZombieBaseState::Enter(PreviousState, TargetPlayer);
	
	Owner->GetController()->StopMovement();
	Owner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Owner->SetLifeSpan(10.0f);
	
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
