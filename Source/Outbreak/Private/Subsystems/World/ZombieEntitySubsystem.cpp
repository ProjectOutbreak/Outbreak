// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/World/ZombieEntitySubsystem.h"

#include "MassCommonFragments.h"
#include "MassEntityConfigAsset.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "MassSpawnerSubsystem.h"
#include "Data/EntityDataTypes.h"
#include "Data/GameData.h"
#include "Data/OutbreakDeveloperSettings.h"
#include "Data/ZombieMassFragments.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/DebugHelper.h"
#include "Utilities/OutbreakStatics.h"

void UZombieEntitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (const auto* Settings = UOutbreakDeveloperSettings::Get())
	{
		if (!Settings->EntityConfig.IsNull())
		{
			EntityConfig = Settings->EntityConfig.LoadSynchronous();
		}
	}
}

void UZombieEntitySubsystem::Deinitialize()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	
	Super::Deinitialize();
}

void UZombieEntitySubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	if (!EntityConfig) return;
	
	EntityTemplate = EntityConfig->GetOrCreateEntityTemplate(InWorld);
	
	if (auto* EntitySub = InWorld.GetSubsystem<UMassEntitySubsystem>())
	{
		EntityManager = &EntitySub->GetMutableEntityManager();
	}
	
	SpawnerSubsystem = InWorld.GetSubsystem<UMassSpawnerSubsystem>();
	
	AssignSpawnPosition();
	RunSpawnPointQuery();
	
	const UOutbreakDeveloperSettings* Settings = UOutbreakDeveloperSettings::Get();
	if (Settings->bAutoActivateEntitySpawn)
	{
		StartSpawnManager();
	}
	
	InWorld.GetTimerManager().SetTimer(TimerHandle_UpdateTargets, this, &ThisClass::UpdateTargetLocations, TargetUpdateInterval, true);
}

void UZombieEntitySubsystem::EnqueueSpawnRequest(const FEntitySpawnRequest& SpawnRequest)
{
	PendingSpawnRequests.Add(SpawnRequest);
}

void UZombieEntitySubsystem::StartSpawnManager()
{
	const auto& WorldContext = GetWorldRef();
	if (!IsValid(&WorldContext)) return;
	
	if (!WorldContext.GetTimerManager().IsTimerActive(TimerHandle_Spawn))
	{
		WorldContext.GetTimerManager().SetTimer(TimerHandle_Spawn, this, &ThisClass::SpawnManager, 0.1f, true);
	}
		
}

void UZombieEntitySubsystem::StopSpawnManager()
{
	const auto& WorldContext = GetWorldRef();
	if (!IsValid(&WorldContext)) return;
	
	WorldContext.GetTimerManager().ClearTimer(TimerHandle_Spawn);
}

void UZombieEntitySubsystem::AssignSpawnPosition()
{
	StartPosition = FVector::ZeroVector;
	
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	
	if (PlayerStarts.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
		StartActor = PlayerStarts[RandomIndex];
		StartPosition = StartActor->GetActorLocation();
	}
}

void UZombieEntitySubsystem::CreateStartEntities(const TArray<FVector>& SpawnPoints)
{
	const UOutbreakDeveloperSettings* Settings = UOutbreakDeveloperSettings::Get();
	if (!Settings) return;
	
	FEntitySpawnRequest SpawnRequest;
	SpawnRequest.Type = EEntityType::Zombie;
	SpawnRequest.Count = Settings->InitialEntityCount;
	SpawnRequest.TotalCount = Settings->InitialEntityCount;
	SpawnRequest.WorldPos = StartPosition;
	SpawnRequest.SpawnPoints = SpawnPoints;
	
	if (Settings->InitialEntityCount > 0)
	{
		EnqueueSpawnRequest(SpawnRequest);
	}
}

void UZombieEntitySubsystem::RunSpawnPointQuery()
{
	const UOutbreakDeveloperSettings* Settings = UOutbreakDeveloperSettings::Get();
	if (!Settings || Settings->SpawnPointQuery.IsNull())
	{
		// Fallback
		CreateStartEntities({});
		return;
	}

	const UEnvQuery* Query = Settings->SpawnPointQuery.LoadSynchronous();
	UEnvQueryManager* EQSManager = UEnvQueryManager::GetCurrent(GetWorld());
	if (!Query || !EQSManager)
	{
		CreateStartEntities({});
		return;
	}
	
	FQueryFinishedSignature Delegate;
	Delegate.BindUObject(this, &ThisClass::OnSpawnPointQueryFinished);

	const FEnvQueryRequest QueryRequest(Query, StartActor);
	EQSManager->RunQuery(QueryRequest, EEnvQueryRunMode::AllMatching, Delegate);
}

void UZombieEntitySubsystem::OnSpawnPointQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	const UOutbreakDeveloperSettings* Settings = UOutbreakDeveloperSettings::Get();
	if (!Settings) return;
	
	TArray<FVector> SpawnPoints;
	
	if (Result.IsValid() && Result->IsSuccessful())
	{
		const int32 NumPoints = FMath::Min(Settings->InitialEntityCount, Result->Items.Num());
		for (int32 i = 0; i < NumPoints; i++)
		{
			SpawnPoints.Add(Result->GetItemAsLocation(i));
		}
	}
	
	CreateStartEntities(SpawnPoints);
}

void UZombieEntitySubsystem::SpawnManager()
{
	ProcessPendingSpawnRequests();
}

void UZombieEntitySubsystem::ProcessPendingSpawnRequests()
{
	if (!EntityManager || !SpawnerSubsystem) return;
	
	const auto* Settings = UOutbreakDeveloperSettings::Get();
	if (!Settings) return;
	
	TArray<FMassEntityHandle> SpawnedEntities;
	int32 SpawnThisTick = Settings->MaxSpawnPerTick;
	for (int i = PendingSpawnRequests.Num() - 1; i >= 0; --i)
	{
		FEntitySpawnRequest& Request = PendingSpawnRequests[i];
		const int32 ThisBatch = FMath::Min(Request.Count, SpawnThisTick);
		if (ThisBatch == 0)
		{
			PendingSpawnRequests.RemoveAtSwap(i);
			continue;
		}
		
		FMassEntityTemplate RequestEntityTemplate;
		switch (Request.Type)
		{
			case EEntityType::Entity:
				// fall through
			case EEntityType::Zombie:
				// fall through
			default: 
				RequestEntityTemplate = EntityTemplate;
				break;;
		}
		
		TArray<FMassEntityHandle> RequestEntities;
		SpawnerSubsystem->SpawnEntities(RequestEntityTemplate, ThisBatch, RequestEntities);
		
		ConfigureSpawnedEntities(Request, RequestEntities);
		
		Request.Count -= ThisBatch;
		SpawnThisTick -= ThisBatch;
		
		if (Request.Count <= 0)
		{
			PendingSpawnRequests.RemoveAtSwap(i);
		}
		
		if (SpawnThisTick <= 0)
		{
			return;
		}
	}
}

void UZombieEntitySubsystem::ConfigureSpawnedEntities(const FEntitySpawnRequest& Request, const TArray<FMassEntityHandle>& RequestEntities)
{
	const int32 SpawnOffset = Request.TotalCount - Request.Count;
	int32 EntityIndex = 0;
	
	for (const auto& Entity : RequestEntities)
	{
		FVector Position = Request.WorldPos;
		const int32 GlobalIndex = SpawnOffset + EntityIndex;
		
		if (Request.SpawnPoints.Num() > 0)
		{
			if (GlobalIndex < Request.SpawnPoints.Num())
			{
				Position = Request.SpawnPoints[GlobalIndex];
			}
		}
		else
		{
			// Need Fallback : Character Spawn Manager
		}
		
	
		if (FTransformFragment* TransformFragment = EntityManager->GetFragmentDataPtr<FTransformFragment>(Entity))
		{
			TransformFragment->GetMutableTransform().SetLocation(Position);
		}
		
		if (FZombieWanderFragment* WanderFragment = EntityManager->GetFragmentDataPtr<FZombieWanderFragment>(Entity))
		{
			WanderFragment->Origin = Position;
		}
		
		if (FZombieEntityFragment* EntityFragment = EntityManager->GetFragmentDataPtr<FZombieEntityFragment>(Entity))
		{
			EntityFragment->TimeToLive = Request.TimeToLive;
		}
		
		const FZombieData* EntityData = UOutbreakStatics::GetZombieData(GetWorld(), EZombieSubType::Runner);
		if (!EntityData)
		{
			continue;
		}
		
		if (FZombieHealthFragment* HealthFragment = EntityManager->GetFragmentDataPtr<FZombieHealthFragment>(Entity))
		{
			HealthFragment->CurrentHealth = EntityData->MaxHealth;
			HealthFragment->MaxHealth = EntityData->MaxHealth;
			HealthFragment->bIsDead = false;
		}
		
		if (FZombieStateFragment* StateFragment = EntityManager->GetFragmentDataPtr<FZombieStateFragment>(Entity))
		{
			StateFragment->CurrentState = EZombieStateType::Idle;
			StateFragment->bIsAlert = false;
		}
		
		if (FZombieCombatFragment* CombatFragment = EntityManager->GetFragmentDataPtr<FZombieCombatFragment>(Entity))
		{
			CombatFragment->AttackDamage = EntityData->AttackDamage;
			CombatFragment->AttackRange = EntityData->AttackRange;
			CombatFragment->AttackRate = EntityData->AttackRate;
		}
		
		if (FZombieMovementFragment* MovementFragment = EntityManager->GetFragmentDataPtr<FZombieMovementFragment>(Entity))
		{
			MovementFragment->MaxWanderSpeed = EntityData->MaxWanderSpeed;
			MovementFragment->MaxRunSpeed = EntityData->MaxRunSpeed;
		}
		
		if (FZombiePerceptionFragment* PerceptionFragment = EntityManager->GetFragmentDataPtr<FZombiePerceptionFragment>(Entity))
		{
			PerceptionFragment->SightRadius = EntityData->SightRadius;
			PerceptionFragment->LoseSightRadius = EntityData->LoseSightRadius;
			PerceptionFragment->PeripheralVisionAngleDegrees = EntityData->PeripheralVisionAngleDegrees;
		}
		
		if (FZombieChaseTargetFragment* TargetFragment = EntityManager->GetFragmentDataPtr<FZombieChaseTargetFragment>(Entity))
		{
			TargetFragment->TargetIndex = FMath::RandRange(0, 100);
		}
		
		EntityIndex++;
	}
}

void UZombieEntitySubsystem::UpdateTargetLocations()
{
	if (GetWorld()->GetNetMode() == NM_Client || !EntityManager)
	{
		return;
	}
	
	TArray<AActor*> PlayerPawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), PlayerPawns);

	TArray<FVector> ValidPlayerLocations;
	for (AActor* Actor : PlayerPawns)
	{
		const APawn* P = Cast<APawn>(Actor);
		if (P && P->IsPlayerControlled())
		{
			ValidPlayerLocations.Add(P->GetActorLocation());
		}
	}

	if (ValidPlayerLocations.Num() == 0 )
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] No valid player locations found for zombies to target."), CURRENT_CONTEXT);
		return;
	}

	FMassEntityQuery SharedUpdateQuery;
	SharedUpdateQuery.AddSharedRequirement<FZombieChaseTargetSharedFragment>(EMassFragmentAccess::ReadWrite);

	FMassExecutionContext Context(*EntityManager);
	SharedUpdateQuery.ForEachEntityChunk(*EntityManager, Context, [&ValidPlayerLocations](FMassExecutionContext& EC)
	{
		FZombieChaseTargetSharedFragment& SharedTarget = EC.GetMutableSharedFragment<FZombieChaseTargetSharedFragment>();
		SharedTarget.TargetLocations = ValidPlayerLocations;
		const FString DebugLocation = FString::JoinBy(ValidPlayerLocations, TEXT("; "), [](const FVector& Vec)
		{
			return Vec.ToCompactString();
		});
	});
}