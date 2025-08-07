// Fill out your copyright notice in the Description page of Project Settings.

#include "FZombieChaseState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Util/Define.h"

FZombieChaseState::FZombieChaseState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm, ACharacterZombie* InOwner): FZombieBaseState(InFsm, EZombieStateType::Chase, InOwner)
{
}

void FZombieChaseState::Enter(EZombieStateType PreviousState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	Super::Enter(PreviousState, TargetPlayer);

	if (!TargetPlayer)
		return;

	const TObjectPtr<UCharacterMovementComponent> MovementComp = Owner->GetCharacterMovement();
	const FZombieData* ZombieData = Owner->GetZombieData();
	
	if (Owner && MovementComp)
	{
		if (ZombieData->bIsCanRun)
			MovementComp->MaxWalkSpeed = ZombieData->MaxRunSpeed;
		else
			MovementComp->MaxWalkSpeed = ZombieData->MaxWalkSpeed;
	}

	// TODO: AcceptanceRadius랑 AttackRange 조정
	const float AcceptanceRadius = ZombieData->AttackRange / 2.0f;

	const TObjectPtr<AZombieAIComponent> ZombieAI = Owner->GetZombieAI();
	const TObjectPtr<UPathFollowingComponent> PathFollowingComp = ZombieAI->GetPathFollowingComponent();
	if (PathFollowingComp)
	{
		PathFollowingComp->SetBlockDetectionState(true);
		PathFollowingComp->SetBlockDetection(BlockDetectionDistance, BlockDetectionInterval, BlockDetectionSampleCount);
	}
	
	DelegateHandle = PathFollowingComp->OnRequestFinished.AddLambda([this](FAIRequestID RequestID, const FPathFollowingResult& Result)
	{
		if (Result.IsSuccess())
		{
			Fsm->ChangeState(EZombieStateType::Attack, CurrentTargetPlayer);
		}
		else if (Result.IsFailure())
		{
			Fsm->ChangeState(EZombieStateType::Alert, CurrentTargetPlayer);
		}
	});
	
	ZombieAI->MoveToActor(CurrentTargetPlayer, AcceptanceRadius, true);
}

void FZombieChaseState::Execute(EZombieStateType CurrentState, float DeltaTime)
{
	Super::Execute(CurrentState, DeltaTime);
}

void FZombieChaseState::Exit(EZombieStateType NextState, TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	Super::Exit(NextState, TargetPlayer);

	const TObjectPtr<AZombieAIComponent> ZombieAI = Owner->GetZombieAI();
	ZombieAI->GetPathFollowingComponent()->OnRequestFinished.Remove(DelegateHandle);
}