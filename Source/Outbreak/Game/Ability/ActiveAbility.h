// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActiveAbility.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActiveAbility : public UInterface
{
	GENERATED_BODY()
};

class OUTBREAK_API IActiveAbility
{
	GENERATED_BODY()

public:
	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
	virtual void CanActivate() const = 0;
};
