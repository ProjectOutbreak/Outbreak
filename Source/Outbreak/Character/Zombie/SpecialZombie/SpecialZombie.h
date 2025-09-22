// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "SpecialZombie.generated.h"

UCLASS()
class OUTBREAK_API ASpecialZombie : public ACharacterZombie
{
	GENERATED_BODY()

public:
	ASpecialZombie();
	
protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRep_Die() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UAbilityComponent> AbilityComponent;
};
