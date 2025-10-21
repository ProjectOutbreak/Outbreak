// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Define.generated.h"

#define CURRENT_CONTEXT *FString(__FUNCTION__)
#define LOG_CALL() UE_LOG(LogTemp, Log, TEXT("[%s] Called"), CURRENT_CONTEXT)
#define LOG_CALL_NUM(Number) UE_LOG(LogTemp, Log, TEXT("[%s] Called %d"), CURRENT_CONTEXT, Number)
#define LOG_CALL_WARNING() UE_LOG(LogTemp, Warning, TEXT("[%s] Called"), CURRENT_CONTEXT)
#define LOG_CALL_WARNING_NUM(Number) UE_LOG(LogTemp, Warning, TEXT("[%s] Called %d"), CURRENT_CONTEXT, Number)
#define LOG_CALL_ERROR() UE_LOG(LogTemp, Error, TEXT("[%s] Called"), CURRENT_CONTEXT)
#define LOG_CALL_ERROR_NUM(Number) UE_LOG(LogTemp, Error, TEXT("[%s] Called %d"), CURRENT_CONTEXT, Number)
#define ECC_Footstep ECollisionChannel::ECC_GameTraceChannel1

UENUM()
enum class EAbilityType : uint8
{
	None,
	DamageEnhance,
	Vibration,
	ToxicAura,
	ToxicAttack,
	SelfDestruct,
};

UENUM()
enum class EMedicineType : uint8
{
	None,
	Medkit,
	Bandage,
	Painkillers,
	Adrenaline,
};

UENUM()
enum class EThrowableType : uint8
{
	None,
	Grenade,
	Molotov,
	FlashBang,
	Shuriken,
};

UENUM()
enum class EFirableType : uint8
{
	None,
	AssaultRifle,
	SniperRifle,
	Pistol,
	Shotgun,
};


UENUM(BlueprintType)
enum class EFireType : uint8
{
	None,
	Single,
	Burst,
	Auto,
};

UENUM()
enum class EWeaponType : uint8
{
	None,
	Firable,
	Throwable,
	Melee,
};

UENUM()
enum class EEquipmentType : uint8
{
	None,
	PrimaryWeapon,
	SecondaryWeapon,
	ThrowableWeapon,
	Medicine,
};

enum class EAvoidanceGroupType : uint8
{
	None = 0,
	Player = 1 << 0,
	Zombie = 1 << 1,
};

UENUM()
enum class ECharacterBodyType : uint8
{
	Normal,
	Muscle,
	Fat,
};

UENUM()
enum class EZombieAnimationType : uint8
{
	None,
	Idle,
	Wander,
	Alert,
	ChaseWalk,
	ChaseRun,
	Attack,
	Stun,
	Die,
};

UENUM()
enum class EZombieStateType : uint8
{
	None	UMETA(DisplayName = "None"),
	Idle	UMETA(DisplayName = "휴면"),
	Wander	UMETA(DisplayName = "배회"),
	Alert	UMETA(DisplayName = "경계"),
	Chase	UMETA(DisplayName = "추적"),
	Attack	UMETA(DisplayName = "공격"),
	Stun	UMETA(DisplayName = "기절"),
	Die 	UMETA(DisplayName = "사망"),
};

UENUM()
enum class EPlayerControlType : uint8
{
	None,
	FirstPersonView,
	TopView,
};

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	None,
	Player UMETA(DisplayName = "플레이어"),
	Zombie UMETA(DisplayName = "좀비"),
};

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	// TODO : Change Real Player Name
	None,
	Player1 UMETA(DisplayName = "플레이어1"),
	Player2 UMETA(DisplayName = "플레이어2"),
	Player3 UMETA(DisplayName = "플레이어3"),
	Player4 UMETA(DisplayName = "플레이어4"),
};

UENUM(BlueprintType)
enum class EZombieType : uint8
{
	None,
	Normal      UMETA(DisplayName = "일반 좀비"),
	Special     UMETA(DisplayName = "특수 좀비"),
	Boss        UMETA(DisplayName = "보스 좀비"),
};

UENUM(BlueprintType)
enum class EZombieSubType : uint8
{
	None,
	Walker      UMETA(DisplayName = "워커"),
	Runner      UMETA(DisplayName = "러너"),
	Bloater     UMETA(DisplayName = "블로터"),
	Soldier     UMETA(DisplayName = "군인 좀비"),
    
	GymRat      UMETA(DisplayName = "헬창"),
	Toxic       UMETA(DisplayName = "톡식"),
	Popper      UMETA(DisplayName = "파퍼"),
	Boomer 		UMETA(DisplayName = "부머"),
	Goo			UMETA(DisplayName = "구"),
	Shield      UMETA(DisplayName = "방패 좀비"),
	Berserker   UMETA(DisplayName = "버서커"),
};

UENUM()
enum class ETimePreset : uint8
{
	Dawn		UMETA(DisplayName = "06:00"),
	Noon		UMETA(DisplayName = "12:00"),
	Dusk		UMETA(DisplayName = "18:00"),
	Midnight	UMETA(DisplayName = "24:00"),
	CurrentTime UMETA(DisplayName = "현재시간 (UTC+9)"),
	RandomOne	UMETA(DisplayName = "랜덤 시간대")
};

class OUTBREAK_API Define { };