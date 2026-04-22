// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/OutbreakDeveloperSettings.h"

UOutbreakDeveloperSettings::UOutbreakDeveloperSettings()
	: bAutoActivateSpawnManager(true),
	bAutoActivateEntitySpawn(true),
	EntityConfig(nullptr),
	MaxSpawnPerTick(64),
	InitialEntityCount(100),
	HeadDamageMultiplier(3.0f),
	BodyDamageMultiplier(1.0f),
	LimbDamageMultiplier(0.7f)
{
}

const UOutbreakDeveloperSettings* UOutbreakDeveloperSettings::Get()
{
	return GetDefault<UOutbreakDeveloperSettings>();
}
