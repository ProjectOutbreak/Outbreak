#include "FZombieWanderState.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Component/ZombieAIComponent.h"
#include "Outbreak/Util/Define.h"

FZombieWanderState::FZombieWanderState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm,
	ACharacterZombie* InOwner): FZombieBaseState(InFsm, EZombieStateType::Wander, InOwner)
{
}

void FZombieWanderState::Enter(const EZombieStateType PreviousState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	Super::Enter(PreviousState, TargetPlayer);
	
	if (Owner && Owner->GetCharacterMovement())
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed = WanderSpeed;
	}

	WanderTimer = FMath::RandRange(MinWanderTime, MaxWanderTime);
	StartWandering();
}

void FZombieWanderState::Execute(const EZombieStateType CurrentState, const float DeltaTime)
{
	Super::Execute(CurrentState, DeltaTime);

	EPathFollowingStatus::Type MoveStatus = AIController->GetMoveStatus();
	WanderTimer -= DeltaTime;
	if (WanderTimer <= 0.0f || MoveStatus == EPathFollowingStatus::Type::Idle)
	{
		float RandomValue = FMath::FRand();
		if (RandomValue < 0.3f)
		{
			ChangeState(EZombieStateType::Idle);
		}
		else
		{
			WanderTimer = FMath::RandRange(MinWanderTime, MaxWanderTime);
			StartWandering();
		}
	}
}

void FZombieWanderState::Exit(const EZombieStateType NextState, const TObjectPtr<ACharacterPlayer> TargetPlayer)
{
	Super::Exit(NextState, TargetPlayer);

	AIController->StopMovement();
}

void FZombieWanderState::StartWandering()
{
	FVector RandomLocation;
	if (FindRandomWanderLocation(RandomLocation))
	{
		AIController->MoveToLocation(RandomLocation, -1.0f, true);
	}
}

bool FZombieWanderState::FindRandomWanderLocation(FVector& OutLocation)
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Owner->GetWorld());
	if (!NavSystem)
		return false;
    
	FVector OriginPosition = Owner->GetActorLocation();
    
	FNavLocation RandomPoint;
	bool bFound = NavSystem->GetRandomReachablePointInRadius(OriginPosition, WanderRadius, RandomPoint);
    
	if (bFound)
	{
		OutLocation = RandomPoint.Location;
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] No random point found in radius"), CURRENT_CONTEXT);
	return false;
}