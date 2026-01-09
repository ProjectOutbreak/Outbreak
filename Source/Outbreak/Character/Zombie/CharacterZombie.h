// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Data/GameData.h"
#include "CharacterZombie.generated.h"

class USphereComponent;
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

	void EnableAttackCollision();
	void DisableAttackCollision();
	
	FOnZombieDeathSignature OnDeathDelegate;

protected:
	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USphereComponent> RightHandCollision;
	
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;
	
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
	UPROPERTY()
	TObjectPtr<USoundCue> DeadSoundCue;
	
public:
	// ~ Begin Getter & Setter
	FORCEINLINE FZombieData* GetZombieData() { return &ZombieData; }
	// ~ End Getter & Setter
};