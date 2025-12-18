// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class UEquipmentController;
enum class EEquipmentType : uint8;
enum class EFireType : uint8;
class UCharacterMovementComponent;
class ACharacterPlayer;
/**
 * 
 */
UCLASS()
class OUTBREAK_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float BlendInterpSpeed = 15.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bShouldMove;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Velocity;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Direction;
	
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	bool bIsOnUse;
	
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	bool bIsOnReload;
	
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	bool bIsSwapIn;
	
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	bool bIsSwapOut;
	
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	float CurrentAmmoInMag;
	
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	EFireType FireType;
	
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	EEquipmentType CurrentEquipmentType;
	
	UPROPERTY(Transient)
	TObjectPtr<ACharacterPlayer> OwnerCharacter;

	UPROPERTY(Transient)
	TWeakObjectPtr<UCharacterMovementComponent> MovementComponent = nullptr;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UEquipmentController> EquipmentController = nullptr;
};