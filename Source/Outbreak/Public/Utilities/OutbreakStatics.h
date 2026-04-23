// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OutbreakStatics.generated.h"

enum class EZombieSubType : uint8;
struct FZombieData;
/**
 * Utility class for Outbreak
 */
UCLASS()
class OUTBREAK_API UOutbreakStatics : public UObject
{
	GENERATED_BODY()
	
public:
	
	/** @return Damage multiplier based on the physical surface type (e.g., head, body, limbs) */
	static float GetDamageMultiplier(EPhysicalSurface SurfaceType);
	
	/** @return Zombie data for the given subtype or nullptr if not found */
	static const FZombieData* GetZombieData(const UObject* WorldContextObject, const EZombieSubType InSubType);
	
};
