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

protected:
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	bool IsAttack;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	bool IsScream;
};
