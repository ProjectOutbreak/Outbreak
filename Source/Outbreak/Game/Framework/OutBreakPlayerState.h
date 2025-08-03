// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OutBreakPlayerState.generated.h"

UENUM(BlueprintType)
enum class ECharacterStatus : uint8
{
	Alive,
	Downed,
	Dead
};

UCLASS()
class OUTBREAK_API AOutBreakPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AOutBreakPlayerState();

	UPROPERTY(ReplicatedUsing = OnRep_ZombieKills)
	int32 ZombieKills;

	UFUNCTION()
	void OnRep_ZombieKills();

	UFUNCTION()
	void AddZombieKill();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
