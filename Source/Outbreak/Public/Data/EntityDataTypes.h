// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntityDataTypes.generated.h"

UENUM()
enum class EEntityType : uint8
{
	Entity,
	Zombie,
};

USTRUCT()
struct FEntitySpawnRequest
{
	GENERATED_BODY()
	
public:
	EEntityType Type = EEntityType::Entity;
	int32 Count = 0;
	int32 TotalCount = 0;
	FVector WorldPos = FVector::ZeroVector;
	float TimeToLive = -1.0f;
	TArray<FVector> SpawnPoints;
};
