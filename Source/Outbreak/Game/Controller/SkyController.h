// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyController.generated.h"

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

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Sky Controller")
	ADirectionalLight* SunLight;

private:
	UPROPERTY()
	ATimeManager* TimeManagerRef;
};
