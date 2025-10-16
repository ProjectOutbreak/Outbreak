// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ZombieAnimInstance.generated.h"

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
	float AttackRate;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly);
	TObjectPtr<ACharacterZombie> OwnerZombie;
};
