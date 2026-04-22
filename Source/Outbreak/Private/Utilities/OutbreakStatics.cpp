// Fill out your copyright notice in the Description page of Project Settings.

#include "Utilities/OutbreakStatics.h"
#include "Data/OutbreakDeveloperSettings.h"

float UOutbreakStatics::GetDamageMultiplier(const EPhysicalSurface SurfaceType)
{
	const UOutbreakDeveloperSettings* Settings = UOutbreakDeveloperSettings::Get();
	
	switch (SurfaceType)
	{
		case SurfaceType1: // Head
			return Settings->HeadDamageMultiplier;
		case SurfaceType2: // Body  
			return Settings->BodyDamageMultiplier;
		case SurfaceType3: // Limbs
			return Settings->LimbDamageMultiplier;
		default:
			return 1.0f;
	}
}
