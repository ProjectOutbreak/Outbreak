// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Define.generated.h"

#define CURRENT_CONTEXT *FString(__FUNCTION__)

UENUM()
enum class EInventorySlotType : uint8
{
	FirstMainWeapon = 0,
	SecondMainWeapon = 1,
	SubWeapon = 2,
	Max,
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

UENUM(BlueprintType)
enum class ECameraMode : uint8
{
	FPS,
	TopView
};

UENUM()
enum class EPlayerControlType : uint8
{
	Shoulder,
	Top,
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
	Walker      UMETA(DisplayName = "걷는 좀비"),
	Runner      UMETA(DisplayName = "뛰는 좀비"),
	Fat         UMETA(DisplayName = "뚱뚱 좀비"),
	Soldier     UMETA(DisplayName = "군인 좀비"),
    
	GymRat      UMETA(DisplayName = "헬창 좀비"),
	Radioactive UMETA(DisplayName = "방사능 좀비"),
	Ghost       UMETA(DisplayName = "유체화 좀비"),
	Shield      UMETA(DisplayName = "방패 좀비"),
	Berserker   UMETA(DisplayName = "광분화 좀비"),
    
    // TODO : Boss Zombie Manage
	Boss_0,
};

class OUTBREAK_API Define { };