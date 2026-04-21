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

UCLASS()
class OUTBREAK_API ACharacterZombie : public ACharacterBase
{
	GENERATED_BODY()

public:
	/** Default constructor */
	ACharacterZombie();
	
	void SetEnableAttackCollision(const bool bEnable);

protected:
	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void SetMesh();

	void ApplyZombieData();
	
	// ~ Begin Replicated Variables
	UFUNCTION()
	void OnRep_ZombieData();
	UPROPERTY(ReplicatedUsing = OnRep_ZombieData)
	FZombieData ZombieData;
	
	UPROPERTY(Replicated)
	bool bIsAttacking = false;
	UPROPERTY(Replicated)
	bool bIsAlert = false;
	// ~ End Replicated Variables
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USphereComponent> RightHandCollision;
	
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;
	
	UPROPERTY()
	AController* LastDamagePlayer;
	
	UPROPERTY()
	TObjectPtr<USoundCue> DeadSoundCue;
	
	EZombieType ZombieType = EZombieType::None;
	EZombieSubType ZombieSubType = EZombieSubType::None;
	float DefaultCapsuleRadius = 10.0f;
	float DefaultCapsuleHalfHeight = 96.0f;
	
protected:

	// ~ Begin ACharacterBase Interface
	virtual void InitCharacterData() override;
	virtual void SetupCollisionAndMesh() override;
	virtual void SetupMovement() override;
	virtual void OnRep_Die() override;
	// ~ End ACharacterBase Interface
	
public:
	
	/// ~ Begin ACharacter Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	// ~ End ACharacter Interface
	
	// ~ Begin APawn Interface
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	// ~ End APawn Interface
	
	// ~ Begin Getter & Setter
	FORCEINLINE FZombieData* GetZombieData() { return &ZombieData; }
	FORCEINLINE bool IsAttacking() const { return bIsAttacking; }
	void SetIsAttacking(const bool bInIsAttacking);
	void SetIsAlert(const bool bInIsAlert);
	// ~ End Getter & Setter
	
};