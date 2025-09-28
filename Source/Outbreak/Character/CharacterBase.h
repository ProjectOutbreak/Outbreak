// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Outbreak/Util/Define.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackHitSignature, AActor*, HitActor, const FHitResult&, HitResult);

UCLASS(Abstract)
class OUTBREAK_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()
	
// --------------------
// Functions
// --------------------
public:
	ACharacterBase();
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void ApplyToxicDamage(float DamagePerSecond, float Duration);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitCharacterData();
	virtual void SetupCollision();
	virtual void SetupMovement();
	virtual bool IsDead() const;

	UFUNCTION()
	virtual void OnRep_CurrentHealth();
	UFUNCTION()
	virtual void OnRep_Die();

	virtual float GetDamageMultiplier(EPhysicalSurface SurfaceType);
	virtual void ApplyDamage(int32 DamageAmount);

	UFUNCTION()
	void OnRep_IsToxic();
	void ApplyToxicTick();
	void ClearToxicEffect();

private:
	void Die();
	
// --------------------
// Variables
// --------------------
public:
	FOnAttackHitSignature OnAttackHit;
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_Die)
	uint8 bIsDead = false;
	
	ECharacterType CharacterType = ECharacterType::None;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	int32 CurrentHealth = 100;

	int32 CurrentExtraHealth = 0;

	// TODO : Hit Damage Multiplier Data Table
	float HeadDamageMultiplier = 3.0f;
	float BodyDamageMultiplier = 1.0f;
	float LimbsDamageMultiplier = 0.7f;

	UPROPERTY(ReplicatedUsing = OnRep_IsToxic)
	bool bIsToxic = false;
	FTimerHandle ToxicTickTimerHandle;
	FTimerHandle ToxicDurationTimerHandle;
	float ToxicDamagePerTick = 0.0f;
};