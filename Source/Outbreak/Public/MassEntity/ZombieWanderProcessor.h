// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "ZombieWanderProcessor.generated.h"

/**
 * 
 */
UCLASS()
class OUTBREAK_API UZombieWanderProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	/** Default constructor */
	UZombieWanderProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
	
};
