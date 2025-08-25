// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "FirableBase.h"
#include "Ak47.generated.h"

UCLASS()
class OUTBREAK_API AAk47 : public AFirableBase
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:
	AAk47();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

// --------------------
// Variables
// --------------------
private:
	UPROPERTY()
	TObjectPtr<class UEquipmentController> EquipmentController;
};