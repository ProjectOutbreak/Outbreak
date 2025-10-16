// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "FZombieBaseState.h"
#include "Outbreak/Util/Define.h"

UENUM()
enum class EChaseType : uint8
{
	Straight,
	Arc,
	Invalid UMETA(Hidden),
};

class OUTBREAK_API FZombieChaseState : public FZombieBaseState
{
public:
	FZombieChaseState(const TSharedPtr<TStateMachine<EZombieStateType, ACharacterPlayer>>& InFsm, ACharacterZombie* InOwner);

	virtual void Enter(EZombieStateType PreviousState, TObjectPtr<ACharacterPlayer> TargetPlayer) override;
	virtual void Execute(EZombieStateType CurrentState, float DeltaTime) override;
	virtual void Exit(EZombieStateType NextState, TObjectPtr<ACharacterPlayer> TargetPlayer) override;

private:
	FDelegateHandle DelegateHandle;
	float BlockDetectionDistance = 10.0f;
	float BlockDetectionInterval = 0.5f;
	int32 BlockDetectionSampleCount = 2;

	EChaseType CurrentChaseType = EChaseType::Invalid;
	float FlankDirection = 1.0f;
	float UpdateTimer = 0.0f;
	const float UpdateInterval = 0.2f;
	const float ArcWeight = 0.6f;
};
