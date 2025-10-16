// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Character/Zombie/State/FZombieStateMachine.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "ZombieAIComponent.generated.h"

struct FAIStimulus;
enum class EZombieStateType : uint8;

UCLASS()
class OUTBREAK_API AZombieAIComponent : public AAIController
{
	GENERATED_BODY()

public:
	AZombieAIComponent();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	void InitializeZombieAI(class ACharacterZombie* InZombie);
	EZombieStateType GetCurrentState() const;
	TObjectPtr<ACharacterPlayer> GetCurrentTargetCharacter() const { return CurrentTargetCharacter; }
	
protected:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
public:
	TSharedPtr<FZombieStateMachine> StateMachine;

protected:
	UPROPERTY()
	TObjectPtr<class ACharacterZombie> OwnerZombie;
	
	UPROPERTY()
	TObjectPtr<UAIPerceptionComponent> AIPerception;
	
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
	
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;
	
	FGenericTeamId TeamId = 1;

private:
	UPROPERTY()
	TObjectPtr<ACharacterPlayer> CurrentTargetCharacter;
};