// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyController.generated.h"

class ASkyLight;
class ADirectionalLight;
class ATimeManager;

UCLASS()
class OUTBREAK_API ASkyController : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkyController();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, Category="Sky Controller")
	ADirectionalLight* SunLight;

	UPROPERTY(EditInstanceOnly, Category="Sky Controller")
	ADirectionalLight* MoonLight;

	UPROPERTY(EditInstanceOnly, Category="Sky Controller")
	ASkyLight* SkylightRef;
	
private:
	UPROPERTY()
	ATimeManager* TimeManagerRef;

	float SunMaxIntensity;
	float MoonMaxIntensity;
	float SkylightMaxIntensity;
};
