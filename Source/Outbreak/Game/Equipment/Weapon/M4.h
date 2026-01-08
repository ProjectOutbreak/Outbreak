// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirableBase.h"
#include "M4.generated.h"

UCLASS()
class OUTBREAK_API AM4 : public AFirableBase
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:
	AM4();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual UTexture2D* GetEquipmentIcon() const override;
// --------------------
// Variables
// --------------------
private:
	
};
