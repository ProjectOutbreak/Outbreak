// Fill out your copyright notice in the Description page of Project Settings.

#include "FZombieChaseState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Component/ZombieAIComponent.h"
#include "Outbreak/Util/Define.h"

FZombieChaseState::FZombieChaseState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm, ACharacterZombie* InOwner): FZombieBaseState(InFsm, EZombieStateType::Chase, InOwner) { }

void FZombieChaseState::Enter(const EZombieStateType PreviousState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	Super::Enter(PreviousState, TargetPlayer);

	const TObjectPtr<UCharacterMovementComponent> MovementComp = Owner->GetCharacterMovement();
	const FZombieData* ZombieData = Owner->GetZombieData();
	const TObjectPtr<AZombieAIComponent> ZombieAI = Owner->GetZombieAI();
	
	if (!TargetPlayer|| !MovementComp || !ZombieData || !ZombieAI) return;
	
	MovementComp->MaxWalkSpeed = ZombieData->MaxRunSpeed;

	if (const TObjectPtr<UPathFollowingComponent> PathFollowingComp = ZombieAI->GetPathFollowingComponent())
	{
		PathFollowingComp->SetBlockDetectionState(true);
		PathFollowingComp->SetBlockDetection(BlockDetectionDistance, BlockDetectionInterval, BlockDetectionSampleCount);
		DelegateHandle = PathFollowingComp->OnRequestFinished.AddLambda([this](FAIRequestID, const FPathFollowingResult& Result)
		{
			if (Result.IsSuccess())
			{
				Fsm->ChangeState(EZombieStateType::Attack, CurrentTargetPlayer);
			}
			// else if (Result.IsFailure())
			// {
				// Fsm->ChangeState(EZombieStateType::Alert, CurrentTargetPlayer);
			// }
		});
	}
	
	CurrentChaseType = FMath::RandBool() ? EChaseType::Arc : EChaseType::Straight;
	if (CurrentChaseType == EChaseType::Straight)
	{
		const float AcceptanceRadius = Owner->GetZombieData()->AttackRange;
		ZombieAI->MoveToActor(CurrentTargetPlayer, AcceptanceRadius, true);
	}
	else if (CurrentChaseType == EChaseType::Arc)
	{
		FlankDirection = FMath::RandBool() ? 1.0f : -1.0f;
		UpdateTimer = 0.0f;
	}
}

void FZombieChaseState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	Super::Execute(CurrentState, DeltaTime);

	if (!Owner || !CurrentTargetPlayer)
	{
		Fsm->ChangeState(EZombieStateType::Alert, nullptr);
		return;
	}
    
	const FVector ZombieLocation = Owner->GetActorLocation();
	const FVector PlayerLocation = CurrentTargetPlayer->GetActorLocation();
	const float DistanceToPlayer = FVector::Dist(ZombieLocation, PlayerLocation);

	if (DistanceToPlayer <= Owner->GetZombieData()->AttackRange)
	{
		Fsm->ChangeState(EZombieStateType::Attack, CurrentTargetPlayer);
		return;
	}

	if (CurrentChaseType == EChaseType::Arc)
	{
		UpdateTimer += DeltaTime;
		if (UpdateTimer >= UpdateInterval)
		{
			UpdateTimer = 0.0f;

			const TObjectPtr<AZombieAIComponent> ZombieAI = Owner->GetZombieAI();
			if (!ZombieAI) return;

			const FVector DirectionToPlayer = (PlayerLocation - ZombieLocation).GetSafeNormal();
			const FVector RightVector = FVector::CrossProduct(DirectionToPlayer, FVector::UpVector);
			const FVector FinalDirection = (DirectionToPlayer + (RightVector * FlankDirection * ArcWeight)).GetSafeNormal();
            
			const float MoveDistance = 500.0f;
			const FVector NextDestination = ZombieLocation + FinalDirection * MoveDistance;
            
			ZombieAI->MoveToLocation(NextDestination, 100.f, true);
		}
	}
}

void FZombieChaseState::Exit(const EZombieStateType NextState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	Super::Exit(NextState, TargetPlayer);
	
	const TObjectPtr<AZombieAIComponent> ZombieAI = Owner->GetZombieAI();
	ZombieAI->GetPathFollowingComponent()->OnRequestFinished.Remove(DelegateHandle);

	if (Owner && Owner->GetZombieAI())
	{
		Owner->GetZombieAI()->StopMovement();
	}
}