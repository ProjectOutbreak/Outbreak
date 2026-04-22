// Fill out your copyright notice in the Description page of Project Settings.


#include "MassEntity/ZombieWanderProcessor.h"

#include "MassCommonFragments.h"
#include "MassLODFragments.h"
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
	EntityQuery.AddRequirement<FZombieWanderFragment>(EMassFragmentAccess::ReadWrite);
	
	EntityQuery.AddRequirement<FZombieDeathFragment>(EMassFragmentAccess::None, EMassFragmentPresence::None);
	
	EntityQuery.AddTagRequirement<FMassHighLODTag>(EMassFragmentPresence::None);
	EntityQuery.AddTagRequirement<FMassOffLODTag>(EMassFragmentPresence::None);
}

void UZombieWanderProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [](FMassExecutionContext& Ctx)
	{
		const auto Transforms = Ctx.GetMutableFragmentView<FTransformFragment>();
		const auto Wanders    = Ctx.GetMutableFragmentView<FZombieWanderFragment>();

		for (int32 i = 0; i < Ctx.GetNumEntities(); ++i)
		{
			FTransform& Transform = Transforms[i].GetMutableTransform();
			FZombieWanderFragment& Wander = Wanders[i];

			Wander.TimeUntilNewTarget -= Ctx.GetDeltaTimeSeconds();

			if (const auto* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Ctx.GetWorld()))
			{
				if (Wander.TimeUntilNewTarget <= 0.f)
				{
					Wander.TimeUntilNewTarget = FMath::FRandRange(3.f, 6.f);

					FNavLocation NavResult;
					if (NavSystem->GetRandomReachablePointInRadius(Wander.Origin, Wander.Radius, NavResult))
					{
						Wander.TargetLocation = NavResult.Location;
					}
				}
			}

			const FVector CurrentLocation = Transform.GetLocation();
			const FVector Delta2D(
				Wander.TargetLocation.X - CurrentLocation.X,
				Wander.TargetLocation.Y - CurrentLocation.Y,
				0.f
			);

			const float DistSq   = Delta2D.SizeSquared();
			const float StepSize = Wander.Speed * Ctx.GetDeltaTimeSeconds();
			if (DistSq < FMath::Square(StepSize)) continue;

			const float   InvDist = FMath::InvSqrt(DistSq);
			const FVector Dir     = Delta2D * InvDist;

			FVector NewLocation = CurrentLocation + Dir * StepSize;
			Transform.SetLocation(NewLocation);
			Transform.SetRotation(Dir.ToOrientationQuat());
		}
	});
}