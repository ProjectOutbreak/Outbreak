// Fill out your copyright notice in the Description page of Project Settings.

#include "MassEntity/ZombieEntity.h"
#include "MassAgentComponent.h"
#include "MassAgentSubsystem.h"
#include "MassEntitySubsystem.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Component/ZombieAIComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/ZombieMassFragments.h"
#include "Utilities/DebugHelper.h"

AZombieEntity::AZombieEntity()
{
	MassAgentComponent = CreateDefaultSubobject<UMassAgentComponent>(TEXT("MassAgent"));
}

void AZombieEntity::OnMassActorActivated()
{
	if (AZombieAIComponent* ZombieAIComp = Cast<AZombieAIComponent>(GetController()))
	{
		ZombieAIComp->StartStateMachineLogic();
	}
}

void AZombieEntity::OnMassActorDeactivated()
{
	if (AZombieAIComponent* ZombieAIComp = Cast<AZombieAIComponent>(GetController()))
	{
		ZombieAIComp->StopStateMachineLogic();
	}
}

void AZombieEntity::OnRep_IsDead()
{
	Super::OnRep_IsDead();
	
	if (HasAuthority())
	{
		UMassEntitySubsystem* EntitySubsystem = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
		if (!EntitySubsystem)
		{
			return;
		}

		const FMassEntityManager& EntityManager = EntitySubsystem->GetMutableEntityManager();

		FZombieDeathFragment DeathFragment;

		EntityManager.Defer().PushCommand<FMassCommandAddFragmentInstances>(
			MassAgentComponent->GetEntityHandle(), DeathFragment
		);
	}
}

void AZombieEntity::BeginPlay()
{
	Super::BeginPlay();

	if (MassAgentComponent)
	{
		if (const auto MassAgentSubsystem = GetWorld()->GetSubsystem<UMassAgentSubsystem>())
		{
			if (MassAgentComponent->GetEntityHandle().IsValid())
			{
				OnEntityAssociated(*MassAgentComponent);
			}

			MassAgentSubsystem->GetOnMassAgentComponentEntityAssociated().AddUObject(this, &ThisClass::OnEntityAssociated);

			MassAgentSubsystem->GetOnMassAgentComponentEntityDetaching().AddUObject(this, &ThisClass::OnEntityDetaching);
		}
	}
}

void AZombieEntity::OnEntityAssociated(const UMassAgentComponent& AgentComponent)
{
	if (!MassAgentComponent || &AgentComponent != MassAgentComponent)
	{
		return;
	}
	
	SyncMassToActor();
	OnMassActorActivated();

	if (auto* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		FNavLocation NavLocation;
		if (NavSystem->ProjectPointToNavigation(GetActorLocation(), NavLocation, FVector(0.f, 0.f, 500.f)))
		{
			FVector CurrentLocation = GetActorLocation();
			CurrentLocation.Z = NavLocation.Location.Z + GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			SetActorLocation(CurrentLocation);
		}
	}
}

void AZombieEntity::OnEntityDetaching(const UMassAgentComponent& AgentComponent)
{
	if (!MassAgentComponent || &AgentComponent != MassAgentComponent)
	{
		return;
	}
	
	SyncActorToMass();
	OnMassActorDeactivated();
}

void AZombieEntity::SyncMassToActor()
{
	const FMassEntityHandle EntityHandle = MassAgentComponent->GetEntityHandle();
	if (!EntityHandle.IsValid())
	{
		return;
	}

	const UMassEntitySubsystem* EntitySubsystem = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
	if (!EntitySubsystem)
	{
		return;
	}

	const FMassEntityManager& EntityManager = EntitySubsystem->GetEntityManager();
	if (const FZombieHealthFragment* HealthFragment = EntityManager.GetFragmentDataPtr<FZombieHealthFragment>(EntityHandle))
	{
		CurrentHealth = HealthFragment->CurrentHealth;
	}
}

void AZombieEntity::SyncActorToMass()
{
	const FMassEntityHandle EntityHandle = MassAgentComponent->GetEntityHandle();
	if (!EntityHandle.IsValid()) 
	{
		return;
	}
	
	UMassEntitySubsystem* EntitySubsystem = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
	if (!EntitySubsystem)
	{
		return;
	}

	const FMassEntityManager& EntityManager = EntitySubsystem->GetMutableEntityManager();
	if (FZombieHealthFragment* HealthFragment = EntityManager.GetFragmentDataPtr<FZombieHealthFragment>(EntityHandle))
	{
		HealthFragment->CurrentHealth = CurrentHealth;
	}
}