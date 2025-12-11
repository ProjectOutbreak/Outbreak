// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ZombieAnimInstance.generated.h"

class UCharacterMovementComponent;
class ACharacterZombie;

UCLASS()
class OUTBREAK_API UZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void PlayScreamingMontage(FOnMontageEnded& OnMontageEndedDelegate);

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UAnimMontage>> AttackMontages;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> ScreamingMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	TObjectPtr<ACharacterZombie> OwnerZombie;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	FVector Velocity;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	float AttackRate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	float GroundSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	float Direction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	bool ShouldMove;

private:
	float GroundSpeedInterpSpeed = 10.0f;
	float DirectionInterpSpeed = 15.0f;
	float CurrentGroundSpeed = 0.0f;
	float CurrentDirection = 0.0f;
};
