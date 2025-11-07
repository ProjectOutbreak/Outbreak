// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Data/GameData.h"
#include "CharacterZombie.generated.h"

class AAIController;
class ACharacterPlayer;
class AZombieAIComponent;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZombieDeathSignature, AActor*, DeadActor);

UCLASS()
class OUTBREAK_API ACharacterZombie : public ACharacterBase
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:
	ACharacterZombie();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void PostInitializeComponents() override;
	
	FORCEINLINE FZombieData* GetZombieData() { return &ZombieData; }
	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }
	FORCEINLINE void SetIsAttacking(const bool NewAttack) { bIsAttacking = NewAttack; }
	FORCEINLINE bool GetIsScreaming() const { return bIsScreaming; }
	FORCEINLINE void SetIsScreaming(const bool NewScream) { bIsScreaming = NewScream; }

	FOnZombieDeathSignature OnDeathDelegate;

protected:
	virtual void InitCharacterData() override;
	virtual void SetupCollision() override;
	virtual void SetupMovement() override;
	virtual void OnRep_Die() override;
	virtual void SetMesh(ECharacterBodyType MeshType);

	UFUNCTION()
	void OnRep_ZombieData();
	void ApplyZombieData();
	
// --------------------
// Variables
// --------------------
protected:
	EZombieType ZombieType = EZombieType::None;
	EZombieSubType ZombieSubType = EZombieSubType::None;
	float DefaultCapsuleRadius = 10.0f;
	float DefaultCapsuleHalfHeight = 96.0f;

	// Replicated Variables
	UPROPERTY(ReplicatedUsing = OnRep_ZombieData)
	FZombieData ZombieData;
	UPROPERTY(Replicated)
	bool bIsAttacking = false;
	UPROPERTY(Replicated)
	bool bIsScreaming = false;
	// ~Replicated Variables

	UPROPERTY()
	AController* LastDamagePlayer;

private:
	TObjectPtr<USoundCue> DeadSoundCue;
};