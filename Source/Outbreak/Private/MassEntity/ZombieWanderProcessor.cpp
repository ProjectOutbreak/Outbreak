// Fill out your copyright notice in the Description page of Project Settings.

#include "MassEntity/ZombieWanderProcessor.h"
#include "MassCommonFragments.h"
#include "MassLODFragments.h"
#include "MassMovementFragments.h"
#include "MassRepresentationFragments.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Data/ZombieMassFragments.h"

UZombieWanderProcessor::UZombieWanderProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::All & ~EProcessorExecutionFlags::Client);
}

void UZombieWanderProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassRepresentationLODFragment>(EMassFragmentAccess::ReadWrite);
	
	EntityQuery.AddRequirement<FZombieWanderFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FZombieDeathFragment>(EMassFragmentAccess::None, EMassFragmentPresence::None);
	
	EntityQuery.AddTagRequirement<FMassHighLODTag>(EMassFragmentPresence::None);
	EntityQuery.AddTagRequirement<FMassOffLODTag>(EMassFragmentPresence::None);
}

void UZombieWanderProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& Ctx)
	{
		const auto Transforms = Ctx.GetMutableFragmentView<FTransformFragment>();
		const auto Wanders = Ctx.GetMutableFragmentView<FZombieWanderFragment>();
		const auto LODFragments = Ctx.GetMutableFragmentView<FMassRepresentationLODFragment>();
		const auto Velocities = Ctx.GetMutableFragmentView<FMassVelocityFragment>();

		for (int32 i = 0; i < Ctx.GetNumEntities(); ++i)
		{
			FTransform& Transform = Transforms[i].GetMutableTransform();
			FZombieWanderFragment& Wander = Wanders[i];
			FMassVelocityFragment& VelocityFragment = Velocities[i];

			const FVector CurrentLocation = Transform.GetLocation();
			Wander.TimeUntilNewTarget -= Ctx.GetDeltaTimeSeconds();

			if (const auto* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Ctx.GetWorld()))
			{
				if (Wander.TimeUntilNewTarget <= 0.f)
				{
					Wander.TimeUntilNewTarget = FMath::FRandRange(3.f, 6.f);

					FNavLocation NavResult;
					if (NavSystem->GetRandomReachablePointInRadius(CurrentLocation, Wander.Radius, NavResult))
					{
						Wander.TargetLocation = NavResult.Location;
					}
				}
			}

			const FVector Delta2D(
				Wander.TargetLocation.X - CurrentLocation.X,
				Wander.TargetLocation.Y - CurrentLocation.Y,
				0.f
			);

			const float DistSq = Delta2D.SizeSquared();
			// const float StepSize = Wander.Speed * Ctx.GetDeltaTimeSeconds();
			const float StepSize = 50.0f * Ctx.GetDeltaTimeSeconds();
			if (DistSq < FMath::Square(StepSize))
			{
				// Idle
				LODFragments[i].LODSignificance = 0.5f; 
				VelocityFragment.Value = FVector::ZeroVector;
				continue;
			}
			
			// Wander
			LODFragments[i].LODSignificance = 1.5f;

			const float InvDist = FMath::InvSqrt(DistSq);
			const FVector Direction = Delta2D * InvDist;
			
			FVector NewLocation = CurrentLocation + Direction * StepSize;
			Transform.SetLocation(NewLocation);
			
			// const FVector CurrentVelocity = Direction * Wander.Speed;
			const FVector CurrentVelocity = Direction * 50.0f;
			VelocityFragment.Value = CurrentVelocity;
		}
	});
}