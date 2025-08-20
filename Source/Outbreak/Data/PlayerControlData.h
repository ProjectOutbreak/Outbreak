// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerControlData.generated.h"

UCLASS()
class OUTBREAK_API UPlayerControlData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPlayerControlData();
	
	UPROPERTY(EditAnywhere, Category = Pawn)
	uint32 bUseControllerRotationYaw : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint32 bOrientRotationToMovement : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint32 bUseControllerDesiredRotation : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	FRotator RotationRate = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	float TargetArmLength = 400.0f;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	FRotator RelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	FVector RelativeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint32 bUsePawnControlRotation : 1;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint32 bInheritPitch : 1;
	
	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint32 bInheritYaw : 1;
	
	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint32 bInheritRoll : 1;
	
	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint32 bDoCollisionTest : 1;
};
