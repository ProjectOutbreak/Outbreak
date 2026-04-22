// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/OutbreakDeveloperSettings.h"

UOutbreakDeveloperSettings::UOutbreakDeveloperSettings()
	: bAutoActivateSpawnManager(true),
	EntityConfig(nullptr),
	MaxSpawnPerTick(64),
	InitialEntityCount(100)
{
}

const UOutbreakDeveloperSettings* UOutbreakDeveloperSettings::Get()
{
	return GetDefault<UOutbreakDeveloperSettings>();
}
