// Fill out your copyright notice in the Description page of Project Settings.


#include "MassEntity/ZombieDeathProcessor.h"
#include "MassRepresentationFragments.h"
#include "Data/ZombieMassFragments.h"

UZombieDeathProcessor::UZombieDeathProcessor() : EntityQuery(*this)
{
	ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::All & ~EProcessorExecutionFlags::Client);
}

void UZombieDeathProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FZombieDeathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadOnly);
}

void UZombieDeathProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager,Context, [](FMassExecutionContext& Ctx)
	{
		const auto Deaths = Ctx.GetMutableFragmentView<FZombieDeathFragment>();
		const auto Reps   = Ctx.GetFragmentView<FMassRepresentationFragment>();

		for (int32 i = 0; i < Ctx.GetNumEntities(); ++i)
		{
			const bool bAlreadySpawnedActor =
					Reps[i].CurrentRepresentation == EMassRepresentationType::HighResSpawnedActor ||
					Reps[i].CurrentRepresentation == EMassRepresentationType::LowResSpawnedActor;

			if (!bAlreadySpawnedActor)
			{
				Ctx.Defer().DestroyEntity(Ctx.GetEntity(i));
				continue;
			}

			Deaths[i].TimeToLive -= Ctx.GetDeltaTimeSeconds();
			if (Deaths[i].TimeToLive <= 0.f)
			{
				Ctx.Defer().DestroyEntity(Ctx.GetEntity(i));
			}
		}
	});
}