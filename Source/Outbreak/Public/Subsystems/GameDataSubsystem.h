// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameDataSubsystem.generated.h"

enum class EZombieSubType : uint8;
struct FZombieData;
/**
 * 
 */
UCLASS()
class OUTBREAK_API UGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	const FZombieData* GetZombieData(const EZombieSubType InSubType) const;
	
private:
	
	TMap<FString, FZombieData*> ZombieDataMap;
    
public:
	// ~ Begin UGameInstanceSubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// ~ End of UGameInstanceSubsystem interface
};
