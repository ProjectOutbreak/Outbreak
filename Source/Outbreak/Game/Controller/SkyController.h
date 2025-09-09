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

private:
	void GetLightsComponent();
	FRotator ComputeMoonRotation(const FRotator& SunRotation) const;
	void ComputeLightAlphas(float CurrentHour, float& SunAlpha, float& MoonAlpha) const;
	void ApplyStarSky(float CurrentHour);
	void ApplyLights(float SunAlpha, float MoonAlpha);
	void ApplySkylights(float SunAlpha, float MoonAlpha);
	void UpdateMoonPosition(const FRotator& MoonRotation) const;


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

	UPROPERTY(EditInstanceOnly, Category="Sky Controller")
	AActor* MoonActor;

	UPROPERTY(EditInstanceOnly, Category="Sky Controller")
	AActor* NightStarSky;
private:
	UPROPERTY()
	ATimeManager* TimeManagerRef;

	float SunMaxIntensity;
	float MoonMaxIntensity;
	float SkylightMaxIntensity;
	const float MoonRadius = 50000.f;
};
