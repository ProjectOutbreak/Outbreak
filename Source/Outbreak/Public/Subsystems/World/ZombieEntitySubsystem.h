// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTemplate.h"
#include "Subsystems/WorldSubsystem.h"
#include "ZombieEntitySubsystem.generated.h"

struct FEnvQueryResult;
class UMassSpawnerSubsystem;
struct FEntitySpawnRequest;
class UMassEntityConfigAsset;

/**
 * 
 */
UCLASS()
class OUTBREAK_API UZombieEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	void EnqueueSpawnRequest(const FEntitySpawnRequest& SpawnRequest);
	
	void StartSpawnManager();
	
	void StopSpawnManager();
	
	
protected:
	
	void AssignSpawnPosition();
	
	void CreateStartEntities(const TArray<FVector>& SpawnPoints);
	
	void RunSpawnPointQuery();
	void OnSpawnPointQueryFinished(TSharedPtr<FEnvQueryResult> Result);
	
	void SpawnManager();
	
	void ProcessPendingSpawnRequests();
	
	void ConfigureSpawnedEntities(const FEntitySpawnRequest& Request, const TArray<FMassEntityHandle>& RequestEntities);
	
	UPROPERTY()
	UMassEntityConfigAsset* EntityConfig;
	
	UPROPERTY()
	UMassSpawnerSubsystem* SpawnerSubsystem;
	
	UPROPERTY()
	TArray<FEntitySpawnRequest> PendingSpawnRequests;
	
	UPROPERTY()
	AActor* StartActor;
	
private:
	
	void UpdateTargetLocations();
	
	const float TargetUpdateInterval = 1.0f;
	
	FTimerHandle TimerHandle_UpdateTargets;
	FTimerHandle TimerHandle_Spawn;
	FMassEntityManager* EntityManager;
	FMassEntityTemplate EntityTemplate;
	FVector StartPosition;
	
public:
	
	// ~ Begin UWorldSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// ~ End UWorldSubsystem Interface
	
};
