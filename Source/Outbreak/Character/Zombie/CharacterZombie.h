// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Data/GameData.h"
#include "CharacterZombie.generated.h"

class ACharacterPlayer;
class AZombieAIComponent;
class USoundCue;

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
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
	FORCEINLINE FZombieData* GetZombieData() { return &ZombieData; }
	FORCEINLINE TObjectPtr<AZombieAIComponent> GetZombieAI() const { return ZombieAI; }
	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }
	FORCEINLINE void SetIsAttacking(const bool NewAttack) { bIsAttacking = NewAttack; }
	FORCEINLINE bool GetIsScreaming() const { return bIsScreaming; }
	FORCEINLINE void SetIsScreaming(const bool NewScream) { bIsScreaming = NewScream; }

protected:
	virtual void InitCharacterData() override;
	virtual void SetupCollision() override;
	virtual void SetupMovement() override;
	virtual void OnRep_Die() override;
	virtual void SetMesh(ECharacterBodyType MeshType);
	
	void ChangeZombieState(EZombieStateType NewState, TObjectPtr<ACharacterPlayer> TargetPlayer = nullptr) const;

// --------------------
// Variables
// --------------------
protected:
	EZombieType ZombieType = EZombieType::None;
	EZombieSubType ZombieSubType = EZombieSubType::None;
	// TODO : modify magic number
	float BodyScale = 1.0f;
	
	UPROPERTY(Replicated)
	FZombieData ZombieData;

	UPROPERTY()
	AController* LastDamagePlayer;

private:
	UPROPERTY()
	TObjectPtr<AZombieAIComponent> ZombieAI;

	TObjectPtr<USoundCue> DeadSoundCue;
	
	bool bIsAttacking = false;
	bool bIsScreaming = false;
};