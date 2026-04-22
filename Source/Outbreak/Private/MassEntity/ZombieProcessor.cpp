// Fill out your copyright notice in the Description page of Project Settings.

#include "MassEntity/ZombieProcessor.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "Data/ZombieMassFragments.h"
#include "MassEntity/ZombieHealthFragment.h"
#include "MassEntity/ZombieStateFragment.h"
#include "Util/Define.h"
#include "Utilities/DebugHelper.h"

UZombieProcessor::UZombieProcessor() : EntityQuery(*this)
{
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Tasks;
}

void UZombieProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FZombieEntityFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::All);
	// EntityQuery.RegisterWithProcessor(*this);
	
	EntityQuery.AddRequirement<FZombieHealthFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FZombieStateFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::All);
}

void UZombieProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& ExecutionContext)
	{
		const int32 EntityCount = ExecutionContext.GetNumEntities();
		const TArrayView<FZombieHealthFragment> HealthList = ExecutionContext.GetMutableFragmentView<FZombieHealthFragment>();
		const TArrayView<FZombieStateFragment> StateList = ExecutionContext.GetMutableFragmentView<FZombieStateFragment>();
		
		const TArrayView<FZombieEntityFragment> EntityFragments = ExecutionContext.GetMutableFragmentView<FZombieEntityFragment>();

		for (int32 i = 0; i < EntityCount; ++i)
		{
			const FMassEntityHandle Entity = ExecutionContext.GetEntity(i);
			FZombieEntityFragment& EntityFragment = EntityFragments[i];
			
			if (EntityFragment.TimeToLive < 0.0f)
			{
				continue;
			}
			
			EntityFragment.TimeToLive -= ExecutionContext.GetDeltaTimeSeconds();
			
			if (EntityFragment.TimeToLive <= 0.0f)
			{
				ExecutionContext.Defer().DestroyEntity(Entity);
				continue;
			}
			
			FZombieHealthFragment& Health = HealthList[i];
			FZombieStateFragment& State = StateList[i];

			// 1. 사망 판정 로직 (기존 CharacterBase의 Die() 역할)
			if (!Health.bIsDead && Health.CurrentHealth <= 0.f)
			{
				Health.bIsDead = true;
				State.CurrentState = EZombieStateType::Die;
				
				// 여기서 필요한 사망 이벤트(사운드, 점수 추가 등)를 발생시킬 수 있습니다.
				PRINT_WITH_CURRENT_CONTEXT(FString::Printf(TEXT("Zombie Entity %d is Dead!"), ExecutionContext.GetEntity(i).Index));
			}
            
			// 2. 간단한 상태 전환 예시
			if (State.CurrentState == EZombieStateType::Idle)
			{
				// 주변에 플레이어가 있는지 확인하는 로직 등을 여기서 일괄 처리
			}
		}
	});
}
