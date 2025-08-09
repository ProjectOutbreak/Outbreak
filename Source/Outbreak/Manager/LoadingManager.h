// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LoadingManager.generated.h"

UCLASS()
class OUTBREAK_API ALoadingManager : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
