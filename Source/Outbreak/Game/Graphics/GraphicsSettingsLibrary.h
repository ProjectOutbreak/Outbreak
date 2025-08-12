// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GraphicsSettingsLibrary.generated.h"

UENUM(BlueprintType)
enum class EOBGraphicsPreset : uint8
{
	Low = 0,
	Medium = 1,
	High = 2,
};

USTRUCT(BlueprintType)
struct FScalabilityPreset
{
	GENERATED_BODY()
    UPROPERTY() int32 ViewDistance = 1;
	UPROPERTY() int32 AntiAliasing = 1;
	UPROPERTY() int32 PostProcess = 1;
	UPROPERTY() int32 Shadows = 1;
	UPROPERTY() int32 GlobalIllimination = 1;
	UPROPERTY() int32 Reflections = 1;
	UPROPERTY() int32 Textures = 1;
	UPROPERTY() int32 Effects = 1;
	UPROPERTY() int32 Foliage = 1;
	UPROPERTY() int32 Shading = 1;
};

UCLASS()
class OUTBREAK_API UGraphicsSettingsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION()
	static void ApplyPreset(EOBGraphicsPreset Preset, bool bSave = true);

	UFUNCTION()
	static void ApplyCustom(const FScalabilityPreset& Custom, bool bSave = true);

	UFUNCTION()
	static int32 GetOverallLevel();

	UFUNCTION()
	static void ApplyDefaultGraphics();
};
