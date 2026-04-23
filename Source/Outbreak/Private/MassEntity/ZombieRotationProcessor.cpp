// Fill out your copyright notice in the Description page of Project Settings.

#include "MassEntity/ZombieRotationProcessor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "Utilities/DebugHelper.h"

UZombieRotationProcessor::UZombieRotationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::All & ~EProcessorExecutionFlags::Client);
}

void UZombieRotationProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
}

void UZombieRotationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& Context)
	{
		const TConstArrayView<FMassVelocityFragment> VelocityList = Context.GetFragmentView<FMassVelocityFragment>();
		const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();

		const int32 NumEntities = Context.GetNumEntities();

		for (int32 i = 0; i < NumEntities; ++i)
		{
			const FVector& Velocity = VelocityList[i].Value;
        
			if (Velocity.SizeSquared() > 10.f)
			{
				FTransform& EntityTransform = TransformList[i].GetMutableTransform();
            
				FRotator TargetRotation = Velocity.Rotation();
				TargetRotation.Yaw -= 90.f;
				TargetRotation.Pitch = 0.f;
				TargetRotation.Roll = 0.f;

				EntityTransform.SetRotation(TargetRotation.Quaternion());
			}
		}
	});
}
