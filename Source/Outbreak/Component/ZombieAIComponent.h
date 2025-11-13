// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Character/Zombie/State/FZombieStateMachine.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
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
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	FORCEINLINE TObjectPtr<ACharacterPlayer> GetTarget() const { return CurrentTargetPlayer; }

private:
	// Setup Functions
	void SetupAIPerception();
	void SetupStateMachine();
	// ~Setup Functions

	// Delegate Handler
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void HandleOwnerDeath(AActor* DeadActor);
	// ~Delegate Handler
	
protected:
	UPROPERTY()
	TObjectPtr<class ACharacterZombie> OwnerZombie;
	
	UPROPERTY()
	TObjectPtr<UAIPerceptionComponent> AIPerception;
	
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	
	TSharedPtr<FZombieStateMachine> StateMachine;
	FGenericTeamId TeamId = 1;

private:
	UPROPERTY(Replicated)
	TObjectPtr<ACharacterPlayer> CurrentTargetPlayer;
};