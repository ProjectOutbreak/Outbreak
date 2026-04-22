// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "ZombieProcessor.generated.h"

UCLASS()
class OUTBREAK_API UZombieProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	/** Default constructor */
    UZombieProcessor();

protected:
	virtual void ConfigureQueries() override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

    FMassEntityQuery EntityQuery;
	
};
