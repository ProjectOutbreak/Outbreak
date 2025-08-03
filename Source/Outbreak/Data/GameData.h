#pragma once
#include "CoreMinimal.h"
#include "Outbreak/Util/Define.h"
#include "StructUtils/InstancedStruct.h"
#include "GameData.generated.h"

USTRUCT(BlueprintType)
struct FSingleEnemyData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float SpawnChance = 1.0f;

	UPROPERTY(EditAnywhere)
	int32 SpawnMin = 1;

	UPROPERTY(EditAnywhere)
	int32 SpawnMax = 1;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;

	UPROPERTY(EditAnywhere)
	FInstancedStruct ExtraData;

	UPROPERTY(EditAnywhere)
	float CapsuleHalfHeight = 0.0f;
};

USTRUCT(BlueprintType)
struct FWaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FSingleEnemyData> Enemies;

	FWaveData()
	{
		Enemies.Empty();
	}
};

USTRUCT(BlueprintType)
struct FWavesData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FWaveData> Waves;
	
	FWavesData()
	{
		Waves.Empty();
	}
};

USTRUCT(BlueprintType)
struct FSpawnerSettingData : public FTableRowBase
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    float SpawnDistanceMin;

    UPROPERTY(EditAnywhere)
    float SpawnDistanceMax;

    UPROPERTY(EditAnywhere)
    int32 MaxEnemies;

    UPROPERTY(EditAnywhere)
    float SpawnInterval;

    UPROPERTY(EditAnywhere)
    bool bUseTagSystem;

    UPROPERTY(EditAnywhere)
    FName Tag;

    UPROPERTY(EditAnywhere)
    float SpawnHeightLimit;

    UPROPERTY(EditAnywhere)
    bool bOptimalHeightSpawn;

    UPROPERTY(EditAnywhere)
    bool bCentralSpawn;

    UPROPERTY(EditAnywhere)
    float CentralSpawnHeightDistance;

    UPROPERTY(EditAnywhere)
    bool bDebug;

    FSpawnerSettingData()
    {
        SpawnDistanceMin = 2000.0f;
        SpawnDistanceMax = 2500.0f;
        MaxEnemies = 200;
        SpawnInterval = 5.0f;
        bUseTagSystem = true;
        Tag = NAME_None;
        SpawnHeightLimit = 2500.0f;
        bOptimalHeightSpawn = true;
        bCentralSpawn = false;
        CentralSpawnHeightDistance = 1000.0f;
        bDebug = false;
    }
};

USTRUCT(BlueprintType)
struct FCharacterBaseData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	float MaxWalkSpeed;
	
	UPROPERTY(VisibleAnywhere)
	float MaxRunSpeed;

	UPROPERTY(VisibleAnywhere)
	uint8 bIsCanRun;

	UPROPERTY(VisibleAnywhere)
	float AttackRate;

	UPROPERTY(VisibleAnywhere)
	int32 RegenRate;

	UPROPERTY(VisibleAnywhere)
	int32 MaxHealth;

	UPROPERTY(VisibleAnywhere)
	int32 MaxExtraHealth;
	
	FCharacterBaseData()
	{
		MaxWalkSpeed = 500.0f;
		MaxRunSpeed = 1000.0f;
		bIsCanRun = false;
		AttackRate = 1.0f;
		RegenRate = 0;
		MaxHealth = 100;
		MaxExtraHealth = 0;
	}
};

USTRUCT(BlueprintType)
struct FPlayerData : public FCharacterBaseData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	EPlayerType PlayerType;
	
	UPROPERTY(VisibleAnywhere)
	int32 BottomHealth;
	
	UPROPERTY(VisibleAnywhere)
	float MaxCrouchSpeed;
	
	FPlayerData()
	{
		PlayerType = EPlayerType::None;
		BottomHealth = 0;
		MaxCrouchSpeed = 200.0f;
	}
};

USTRUCT(BlueprintType)
struct FZombieData : public FCharacterBaseData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	EZombieType ZombieType;
	
	UPROPERTY(VisibleAnywhere)
	EZombieSubType ZombieSubType;
	
	UPROPERTY(VisibleAnywhere)
	float MaxWanderSpeed;
	
	UPROPERTY(VisibleAnywhere)
	int32 AttackDamage;
	
	UPROPERTY(VisibleAnywhere)
	float AttackRange;
	
	UPROPERTY(VisibleAnywhere)
	float SightRadius;
	
	UPROPERTY(VisibleAnywhere)
	float LoseSightRadius;
	
	UPROPERTY(VisibleAnywhere)
	float PeripheralVisionAngleDegrees;
	
	UPROPERTY(VisibleAnywhere)
	float TurnRate;
	
	FZombieData()
	{
		ZombieType = EZombieType::None;
		ZombieSubType = EZombieSubType::None;
		MaxWanderSpeed = 300.0f;
		AttackDamage = 10;
		AttackRange = 100.0f;
		SightRadius = 2000.0f;
		LoseSightRadius = 2500.0f;
		PeripheralVisionAngleDegrees = 90.0f;
		TurnRate = 5.0f;
	}
};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TraceMaxDistance = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Recoil")
	float BulletSpread = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Sound")
	USoundBase* ShotSound = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Weapon")
	float FireFrequency = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Ammo")
	int32 MagazineCapacity = 0;

	UPROPERTY(EditAnywhere, Category="Weapon|Ammo")
	int32 TotalAmmo = 0;

	UPROPERTY(VisibleAnywhere, Category="Weapon|Ammo")
	int32 CurrentAmmo = 0;

	UPROPERTY(VisibleAnywhere, Category="Weapon|Ammo")
	int32 MaxAmmo = 0;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Ammo")
	float ReloadDuration = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Ammo")
	bool bIsReloading = false;
};

class OUTBREAK_API GameData { };