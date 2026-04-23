// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "OutbreakDeveloperSettings.generated.h"

class UMassEntityConfigAsset;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Outbreak Developer Settings"))
class OUTBREAK_API UOutbreakDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	
	/** Default constructor. */
	UOutbreakDeveloperSettings();

	/**
	 * Get the global Trombone UI settings object.
	 */
	static const UOutbreakDeveloperSettings* Get();
	
public:
	
	UPROPERTY(Config, NoClear, EditAnywhere, BlueprintReadOnly, Category = "System Settings")
	bool bAutoActivateSpawnManager;
	
	UPROPERTY(Config, NoClear, EditAnywhere, BlueprintReadOnly, Category = "System Settings")
	bool bAutoActivateEntitySpawn;
	
	UPROPERTY(Config, NoClear, EditAnywhere, BlueprintReadOnly, Category = "System Settings")
	TSoftObjectPtr<UMassEntityConfigAsset> EntityConfig;
	
	UPROPERTY(Config, NoClear, EditAnywhere, BlueprintReadOnly, Category = "System Settings")
	TSoftObjectPtr<UEnvQuery> SpawnPointQuery;
	
	UPROPERTY(Config, NoClear, EditAnywhere, BlueprintReadOnly, Category = "System Settings", meta = (ClampMin = "1"))
	int32 MaxSpawnPerTick;
	
	UPROPERTY(Config, NoClear, EditAnywhere, BlueprintReadOnly, Category = "System Settings", meta = (ClampMin = "0"))
	int32 InitialEntityCount;
	
public:
	
	UPROPERTY(Config, NoClear, EditAnywhere, BlueprintReadOnly, Category = "System Settings")
	TSoftObjectPtr<UDataTable> ZombieDataTable;
	
public:
	
	UPROPERTY(Config, NoClear, EditAnywhere, BlueprintReadOnly, Category = "Game Balance", meta = (ClampMin = "0"))
	float HeadDamageMultiplier;
	
	UPROPERTY(Config, NoClear, EditAnywhere, BlueprintReadOnly, Category = "Game Balance", meta = (ClampMin = "0"))
	float BodyDamageMultiplier;
	
	UPROPERTY(Config, NoClear, EditAnywhere, BlueprintReadOnly, Category = "Game Balance", meta = (ClampMin = "0"))
	float LimbDamageMultiplier;
	
};
