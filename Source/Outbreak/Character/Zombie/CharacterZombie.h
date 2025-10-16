// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Component/ZombieAIComponent.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Util/Define.h"
#include "CharacterZombie.generated.h"

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
	virtual void PostInitializeComponents() override;
	
	FORCEINLINE FZombieData* GetZombieData() { return &ZombieData; }
	FORCEINLINE TObjectPtr<AZombieAIComponent> GetZombieAI() const { return ZombieAI; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void InitCharacterData() override;
	virtual void SetupCollision() override;
	virtual void SetupMovement() override;
	virtual void OnRep_Die() override;
	virtual void SetMesh(ECharacterBodyType MeshType);
	void ChangeZombieState(EZombieStateType NewState, TObjectPtr<ACharacterPlayer> TargetPlayer = nullptr) const;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,class AActor* DamageCauser) override;

// --------------------
// Variables
// --------------------
public:
	UPROPERTY(BlueprintReadOnly)
	bool bIsAttacking = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsScreaming = false;
	
protected:
	EZombieType ZombieType = EZombieType::None;
	EZombieSubType ZombieSubType = EZombieSubType::None;
	// TODO : modify magic number
	float BodyScale = 1.0f;
	
	UPROPERTY(Replicated)
	FZombieData ZombieData;

	UPROPERTY()
	AController* LastDamagePlayer;

	TMap<EZombieAnimationType, TArray<FName>> AnimSectionMap;

private:
	UPROPERTY()
	TObjectPtr<AZombieAIComponent> ZombieAI;
};