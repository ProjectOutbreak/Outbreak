// Fill out your copyright notice in the Description page of Project Settings.

#include "FZombieBaseState.h"
#include "Outbreak/Animation/ZombieAnimInstance.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Component/ZombieAIComponent.h"
#include "Outbreak/Util/Define.h"

FZombieBaseState::FZombieBaseState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, const EZombieStateType InStateKey, ACharacter* InOwner)
	: TState(InFsm, InStateKey)
{
	Owner = InOwner;
	OwnerCharacter = InOwner;
	AIController = Cast<AZombieAIComponent>(InOwner->GetController());
}

void FZombieBaseState::Enter(EZombieStateType PreviousState) { }

void FZombieBaseState::Execute(EZombieStateType CurrentState, float DeltaTime) { }

void FZombieBaseState::Exit(EZombieStateType NextState) { }

TObjectPtr<UZombieAnimInstance> FZombieBaseState::GetAnimInstance()
{
	if (!CachedAnimInstance && Owner)
	{
		CachedAnimInstance = Cast<UZombieAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	}
	return CachedAnimInstance;
}

TObjectPtr<ACharacterPlayer> FZombieBaseState::GetTarget() const
{
	if (AIController)
	{
		return AIController->GetTarget(); 
	}
	return nullptr;
}
