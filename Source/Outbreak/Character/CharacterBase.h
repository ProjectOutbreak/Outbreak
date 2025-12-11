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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void BeginPlay() override;
	
	void ApplyToxicDamage(float DamagePerSecond, float Duration);
	void ApplyHeal(float HealAmount);
	
	bool IsDead() const;
	
	UFUNCTION(BlueprintCallable) void TriggerFootStepLeft();
	UFUNCTION(BlueprintCallable) void TriggerFootStepRight();
protected:
	virtual void InitCharacterData();
	virtual void SetupCollision();
	virtual void SetupMovement();
	virtual float GetDamageMultiplier(EPhysicalSurface SurfaceType);
	
	void ApplyDamage(int32 DamageAmount);
	void ApplyToxicTick();
	void ClearToxicEffect();

private:
	void Die();
	void OnRagdoll();
	
// --------------------
// Variables
// --------------------
public:
	FOnAttackHitSignature OnAttackHit;
	
protected:
	ECharacterType CharacterType = ECharacterType::None;
	int32 CurrentExtraHealth = 0;

	// TODO : Hit Damage Multiplier Data Table
	float HeadDamageMultiplier = 3.0f;
	float BodyDamageMultiplier = 1.0f;
	float LimbsDamageMultiplier = 0.7f;

	FTimerHandle ToxicTickTimerHandle;
	FTimerHandle ToxicDurationTimerHandle;
	float ToxicDamagePerTick = 0.0f;

protected:
	// RepNotify
	UFUNCTION()
	virtual void OnRep_Die();
	UFUNCTION()
	virtual void OnRep_CurrentHealth();
	UFUNCTION()
	void OnRep_IsToxic();
	
	UPROPERTY(ReplicatedUsing = OnRep_Die)
	uint8 bIsDead : 1 = 0;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	int32 CurrentHealth = 100;
	UPROPERTY(ReplicatedUsing = OnRep_IsToxic)
	bool bIsToxic = false;
	// ~RepNotify

	UPROPERTY() TObjectPtr<class UFootStepComponent> FootStepComponent;
};