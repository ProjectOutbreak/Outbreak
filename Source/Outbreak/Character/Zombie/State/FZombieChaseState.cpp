// Fill out your copyright notice in the Description page of Project Settings.

#include "FZombieChaseState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Component/ZombieAIComponent.h"
#include "Outbreak/Util/Define.h"

FZombieChaseState::FZombieChaseState(const TSharedPtr<TStateMachine<EZombieStateType>>& InFsm, ACharacterZombie* InOwner): FZombieBaseState(InFsm, EZombieStateType::Chase, InOwner) { }

void FZombieChaseState::Enter(const EZombieStateType PreviousState)
{
	Super::Enter(PreviousState);

	const TObjectPtr<UCharacterMovementComponent> MovementComp = Owner->GetCharacterMovement();
	const FZombieData* ZombieData = Owner->GetZombieData();
	
	if (!GetTarget() || !MovementComp || !ZombieData || !AIController) return;
	
	MovementComp->MaxWalkSpeed = ZombieData->MaxRunSpeed;

	if (const TObjectPtr<UPathFollowingComponent> PathFollowingComp = AIController->GetPathFollowingComponent())
	{
		PathFollowingComp->SetBlockDetectionState(true);
		PathFollowingComp->SetBlockDetection(BlockDetectionDistance, BlockDetectionInterval, BlockDetectionSampleCount);
		DelegateHandle = PathFollowingComp->OnRequestFinished.AddLambda([this](FAIRequestID, const FPathFollowingResult& Result)
		{
			if (Result.IsSuccess())
			{
				Fsm->ChangeState(EZombieStateType::Attack);
			}
			// else if (Result.IsFailure())
			// {
				// Fsm->ChangeState(EZombieStateType::Alert);
			// }
		});
	}
	
	CurrentChaseType = FMath::RandBool() ? EChaseType::Arc : EChaseType::Straight;
	if (CurrentChaseType == EChaseType::Straight)
	{
		const float AcceptanceRadius = Owner->GetZombieData()->AttackRange;
		AIController->MoveToActor(GetTarget(), AcceptanceRadius, true);
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

	const TObjectPtr<ACharacterPlayer> CurrentTarget = GetTarget();

	if (!Owner || !CurrentTarget)
	{
		Fsm->ChangeState(EZombieStateType::Alert);
		return;
	}
    
	const FVector ZombieLocation = Owner->GetActorLocation();
	const FVector PlayerLocation = CurrentTarget->GetActorLocation();
	const float DistanceToPlayer = FVector::Dist(ZombieLocation, PlayerLocation);

	if (DistanceToPlayer <= Owner->GetZombieData()->AttackRange)
	{
		Fsm->ChangeState(EZombieStateType::Attack);
		return;
	}

	if (CurrentChaseType == EChaseType::Arc)
	{
		UpdateTimer += DeltaTime;
		if (UpdateTimer >= UpdateInterval)
		{
			UpdateTimer = 0.0f;
			
			const FVector DirectionToPlayer = (PlayerLocation - ZombieLocation).GetSafeNormal();
			const FVector RightVector = FVector::CrossProduct(DirectionToPlayer, FVector::UpVector);
			const FVector FinalDirection = (DirectionToPlayer + (RightVector * FlankDirection * ArcWeight)).GetSafeNormal();
            
			const float MoveDistance = 500.0f;
			const FVector NextDestination = ZombieLocation + FinalDirection * MoveDistance;
            
			AIController->MoveToLocation(NextDestination, 100.f, true);
		}
	}
}

void FZombieChaseState::Exit(const EZombieStateType NextState)
{
	Super::Exit(NextState);

	AIController->GetPathFollowingComponent()->OnRequestFinished.Remove(DelegateHandle);
	AIController->StopMovement();
}