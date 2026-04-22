// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieInterface.h"
#include "Character/CharacterBase.h"
#include "Components/SphereComponent.h"
#include "Data/GameData.h"
#include "GameFramework/Character.h"
#include "ZombieEntityBase.generated.h"

enum class ECharacterBodyType : uint8;

/**
 * Replaced according to the LOD in the CrowdVisualization Trait
 */
UCLASS()
class OUTBREAK_API AZombieEntityBase : public ACharacter, public IZombieInterface
{
	GENERATED_BODY()

public:
	
	/** Default constructor */
	AZombieEntityBase();
	
	FOnAttackOtherCharacterSignature OnAttackOtherCharacter;
	
	FOnCharacterDeathSignature OnCharacterDeathDelegate;

public:
	virtual void SetEnableAttackCollision(const bool bEnable) override;

	virtual void SetIsAttacking(const bool bInIsAttacking) override; // TODO : 인터페이스에서 빼기
	
protected:
	
	void OnRagdoll() const;
	
	void ApplyDamage(int32 InDamageAmount);
	
	void Die();
	
protected:

	UFUNCTION()
	virtual void OnRep_IsDead();
	
	UPROPERTY(ReplicatedUsing = OnRep_IsDead)
	uint8 bIsDead : 1 = 0;
	
	UPROPERTY(Replicated)
	int32 CurrentHealth = 100;
	
	UPROPERTY(Replicated)
	FZombieData ZombieData;
	
	UPROPERTY(Replicated)
	bool bIsAttacking = false;
	
protected:
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> DeadSoundCue;
	
	UPROPERTY()
	TObjectPtr<UFootStepComponent> FootStepComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USphereComponent> RightHandCollision;
	
	UPROPERTY(EditDefaultsOnly)
	float DefaultCapsuleRadius = 10.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float DefaultCapsuleHalfHeight = 96.0f;

private:
	
	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;
	
	UPROPERTY()
	TWeakObjectPtr<AController> LastDamagedPlayer;
	
public:
	
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
public:
	
	FORCEINLINE bool IsDead() const { return bIsDead || CurrentHealth <= 0; }
	virtual FZombieData* GetZombieData() override { return &ZombieData; }
	virtual bool IsAttacking() const override { return bIsAttacking; }
	virtual void SetIsAlert(const bool bInIsAlert) override { /* Not Implemented */ }
	virtual ACharacter* GetCharacter() override { return this; }

};
