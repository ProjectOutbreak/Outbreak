// Fill out your copyright notice in the Description page of Project Settings.

#include "MassEntity/ZombieProcessor.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "Data/ZombieMassFragments.h"
#include "Steering/MassSteeringFragments.h"
#include "Util/Define.h"

UZombieProcessor::UZombieProcessor() : EntityQuery(*this)
{
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Tasks;
}

void UZombieProcessor::ConfigureQueries()
{
	// EntityQuery.AddRequirement<FZombieHealthFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All);
	// EntityQuery.AddRequirement<FZombieStateFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All);
	// EntityQuery.AddRequirement<FZombieCombatFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All);
	// EntityQuery.AddRequirement<FZombiePerceptionFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All);
	
	EntityQuery.AddRequirement<FZombieMovementFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FZombieChaseTargetFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All);
	EntityQuery.AddSharedRequirement<FZombieChaseTargetSharedFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All);
	
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::All);
}

void UZombieProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& ExecutionContext)
	{
		const FZombieChaseTargetSharedFragment& SharedTarget = ExecutionContext.GetSharedFragment<FZombieChaseTargetSharedFragment>();
		const int32 NumPlayers = SharedTarget.TargetLocations.Num();

		if (NumPlayers == 0)
		{
			return;
		}
		
		const auto MoveTargetList = ExecutionContext.GetMutableFragmentView<FMassMoveTargetFragment>();
		const auto TransformList = ExecutionContext.GetFragmentView<FTransformFragment>();
		const auto TargetList = ExecutionContext.GetFragmentView<FZombieChaseTargetFragment>();
		const auto MovementList = ExecutionContext.GetFragmentView<FZombieMovementFragment>();

		for (int32 i = 0; i < ExecutionContext.GetNumEntities(); ++i)
		{
			const int32 Index = TargetList[i].TargetIndex % NumPlayers;
			const FVector TargetPos = SharedTarget.TargetLocations[Index];
			const FVector CurrentPos = TransformList[i].GetTransform().GetLocation();

			FMassMoveTargetFragment& MoveTarget = MoveTargetList[i];

			// MoveTarget.Center = TargetPos;
			// MoveTarget.DistanceToGoal = FVector::Dist(TargetPos, CurrentPos);
			// MoveTarget.Forward = (TargetPos - CurrentPos).GetSafeNormal();
			// MoveTarget.DesiredSpeed = FMassInt16Real(MovementList[i].MaxRunSpeed);
			//
			// MoveTarget.SlackRadius = 500.0f;
			//
			// if (MoveTarget.GetCurrentAction() != EMassMovementAction::Move)
			// {
			// 	MoveTarget.CreateNewAction(EMassMovementAction::Move, *GetWorld());
			// }
		}
	});
	
}
