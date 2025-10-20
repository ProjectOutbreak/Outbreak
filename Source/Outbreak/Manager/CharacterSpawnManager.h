// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Util/Define.h"
#include "CharacterSpawnManager.generated.h"

class ACharacterPlayer;

UCLASS()
class OUTBREAK_API ACharacterSpawnManager : public AActor
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:	
	FZombieData* GetZombieData(const EZombieSubType Type);
	FPlayerData* GetPlayerData(const EPlayerType Type);
	
	void SetSettingId(FName InSettingId);
	void SetWaveId(FName InWaveId);

	void Activate(const TObjectPtr<ACharacterPlayer>& InTarget);
	void Deactivate();
	
protected:
	virtual void BeginPlay() override;

private:
	FORCEINLINE FSpawnerSettingData* GetCurrentSettingData() { return &SpawnerSettingData; }
	FORCEINLINE FWavesData* GetCurrentWaveData() { return &WavesData; }
	void UpdateSettingData();
	void UpdateWaveData();
	void ClampSettingDataValues(FSpawnerSettingData& Setting);

	FORCEINLINE bool CheckSpawnChance(const float Chance) const { return FMath::FRand() <= Chance; }
	FORCEINLINE int32 GetRandomSpawnCount(const int32 Min, const int32 Max) const { return FMath::RandRange(Min, Max); }
	FORCEINLINE bool IsExceededEnemyLimit() const { return SpawnedEnemies >= SpawnerSettingData.MaxEnemies; }
	
	FWaveData GetWaveData(const int32 WaveIndex);
	FVector FindRandomSpawnLocation(float MinDistance, float MaxDistance);
	FVector GetRandomLocationInRadius(const FVector& OptimalHeight, float Radius, bool bDebug) const;
	void SpawnEnemies();

// --------------------
// Variables
// --------------------
private:
	UPROPERTY()
	TObjectPtr<ACharacter> Target;
	
	int32 SpawnedEnemies = 0;

	FSpawnerSettingData SpawnerSettingData;
	FWavesData WavesData;
	
	FName SpawnerSettingId = FName("Default");
	FName WaveId = FName("0");
	int32 CurrentWaveIndex = 0;
	
	TMap<FString, FZombieData*> ZombieDataMap;
	TMap<FString, FPlayerData*> PlayerDataMap;

	FTimerHandle SpawnTimerHandle;
	bool bIsActivated = false;

	// Data Tables
	UPROPERTY(EditAnywhere, Category="Data Tables")
	TObjectPtr<UDataTable> ZombieDataTable;
	UPROPERTY(EditAnywhere, Category="Data Tables")
	TObjectPtr<UDataTable> PlayerDataTable;
	UPROPERTY(EditAnywhere, Category="Data Tables")
	TObjectPtr<UDataTable> SpawnerSettingDataTable;
	UPROPERTY(EditAnywhere, Category="Data Tables")
	TObjectPtr<UDataTable> WaveDataTable;
	// ~Data Tables
};