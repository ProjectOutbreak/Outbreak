// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "ZombieMassFragments.generated.h"

USTRUCT()
struct FZombieEntityFragment : public FMassFragment
{
	GENERATED_BODY()
	
	FVector Position;
	FVector Forward;
	float TimeToLive = 0.f;
	
};
