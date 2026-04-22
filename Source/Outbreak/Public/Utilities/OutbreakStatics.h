// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OutbreakStatics.generated.h"

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
	
};
