#pragma once
#include "CoreMinimal.h"
#include "Outbreak/Util/Define.h"
#include "StructUtils/InstancedStruct.h"
#include "Sound/SoundCue.h"
#include "GameData.generated.h"

USTRUCT()
struct FSingleEnemyData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float SpawnChance = 1.0f;

	UPROPERTY(EditAnywhere)
	int32 SpawnMin = 1;

	UPROPERTY(EditAnywhere)
	int32 SpawnMax = 2;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class = nullptr;

	UPROPERTY(EditAnywhere)
	FInstancedStruct ExtraData;

	UPROPERTY(EditAnywhere)
	float CapsuleHalfHeight = 0.0f;
};

USTRUCT()
struct FWaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FSingleEnemyData> Enemies;
};

USTRUCT()
struct FWavesData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FWaveData> Waves;
};

USTRUCT()
struct FSpawnerSettingData : public FTableRowBase
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    float SpawnDistanceMin = 2000.0f;

    UPROPERTY(EditAnywhere)
    float SpawnDistanceMax = 2500.0f;

    UPROPERTY(EditAnywhere)
    int32 MaxEnemies = 100;

    UPROPERTY(EditAnywhere)
    float SpawnInterval = 5.0f;

    UPROPERTY(EditAnywhere)
    bool bUseTagSystem = true;

    UPROPERTY(EditAnywhere)
    FName Tag = "Spawn";

    UPROPERTY(EditAnywhere)
    float SpawnHeightLimit = 2500.0f;

    UPROPERTY(EditAnywhere)
    bool bOptimalHeightSpawn = true;

    UPROPERTY(EditAnywhere)
    bool bCentralSpawn = false;

    UPROPERTY(EditAnywhere)
    float CentralSpawnHeightDistance = 1000.0f;

    UPROPERTY(EditAnywhere)
    bool bDebug = false;
};

USTRUCT()
struct FCharacterBaseData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float MaxWalkSpeed = 500.0f;
	
	UPROPERTY(EditAnywhere)
	float MaxRunSpeed = 1000.0f;

	UPROPERTY(EditAnywhere)
	uint8 bIsCanRun = 0;

	UPROPERTY(EditAnywhere)
	float AttackRate = 1.0f;

	UPROPERTY(EditAnywhere)
	int32 RegenRate = 0;

	UPROPERTY(EditAnywhere)
	int32 MaxHealth = 100;

	UPROPERTY(EditAnywhere)
	int32 MaxExtraHealth = 0;
};

USTRUCT()
struct FPlayerData : public FCharacterBaseData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EPlayerType PlayerType = EPlayerType::None;
	
	UPROPERTY(EditAnywhere)
	int32 BottomHealth = 0;
	
	UPROPERTY(EditAnywhere)
	float MaxCrouchSpeed = 200.0f;
};

USTRUCT()
struct FZombieData : public FCharacterBaseData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EZombieType ZombieType = EZombieType::None;
    
	UPROPERTY(EditAnywhere)
	EZombieSubType ZombieSubType = EZombieSubType::None;
    
	UPROPERTY(EditAnywhere)
	float MaxWanderSpeed = 300.0f;
    
	UPROPERTY(EditAnywhere)
	int32 AttackDamage = 10;
    
	UPROPERTY(EditAnywhere)
	float AttackRange = 100.0f;
    
	UPROPERTY(EditAnywhere)
	float SightRadius = 2000.0f;
    
	UPROPERTY(EditAnywhere)
	float LoseSightRadius = 2500.0f;
    
	UPROPERTY(EditAnywhere)
	float PeripheralVisionAngleDegrees = 90.0f;
    
	UPROPERTY(EditAnywhere)
	float TurnRate = 5.0f;
};

USTRUCT()
struct FMedicineData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float HealAmount = 0.0f;
	
	UPROPERTY(EditAnywhere)
	float UseTime = 0.0f;

	UPROPERTY(EditAnywhere)
	bool bCanUseSelf = true;

	UPROPERTY(EditAnywhere)
	bool bCanUseOthers = true;
};

USTRUCT()
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float Damage = 0.0f;
};

USTRUCT()
struct FFirableData : public FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float FireRate = 0.0f;
	
	UPROPERTY(EditAnywhere)
	float VerticalRecoil = 0.0f;

	UPROPERTY(EditAnywhere)
	float HorizontalRecoil = 0.0f;

	UPROPERTY(EditAnywhere)
	float Ergonomics = 0.0f;
	
	UPROPERTY(EditAnywhere)
	float Accuracy = 0.0f;
	
	UPROPERTY(EditAnywhere)
	float MuzzleVelocity = 0.0f;

	UPROPERTY(EditAnywhere)
	TArray<EFireType> FireTypes;
	
	UPROPERTY(EditAnywhere)
	float TraceMaxDistance = 0.0f;
	
	UPROPERTY(EditAnywhere)
	int32 MagazineSize = 0;

	UPROPERTY(EditAnywhere)
	int32 MaxAmmo = 0;
	
	UPROPERTY(EditAnywhere)
	float ReloadTime = 0.0f;
};

USTRUCT()
struct FThrowableData : public FWeaponData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	int32 MaxCount = 0;

	UPROPERTY(EditAnywhere)
	float ThrowForce = 0.0f;

	UPROPERTY(EditAnywhere)
	float ExplosionRadius = 0.0f;

	UPROPERTY(EditAnywhere)
	float FuseTime = 0.0f;
};

USTRUCT()
struct FEquipmentSounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> EquipSound = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> UnequipSound = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> UseSound = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ReloadSound = nullptr;
};

class OUTBREAK_API GameData { };