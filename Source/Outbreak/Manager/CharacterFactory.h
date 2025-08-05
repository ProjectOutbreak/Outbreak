// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "CharacterSpawnManager.h"
#include "CharacterFactory.generated.h"

class ACharacterBase;

UCLASS()
class OUTBREAK_API UCharacterFactory : public UObject
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:
	UCharacterFactory();
	bool SpawnCharacter(const TSubclassOf<AActor>& SpawnClass, const FTransform& SpawnTransform, const FActorSpawnParameters& SpawnParams);
	
// --------------------
// Variables
// --------------------
private:
	TMap<EPlayerType, TSubclassOf<ACharacterBase>> PlayerClassMap;
};