// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OBPlayerController.generated.h"

UCLASS()
class OUTBREAK_API AOBPlayerController : public APlayerController
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:
	AOBPlayerController();


protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(class APawn* PossessedPawn) override;
	virtual void SetupInputComponent() override;
	void PerformInteract();

private:
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void Jump();
	void StopJumping();
	void Run();
	void StopRun();
	void Crouch();
	void StopCrouch();



// --------------------
// Variables
// --------------------
	UPROPERTY()
	TObjectPtr<class ACharacterPlayer> ControlledCharacter;

	UPROPERTY()
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY()
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY()
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY()
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction> InteractAction;
	

	UPROPERTY(EditAnywhere, Category = "Config|Movement")
	float WalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Config|Movement")
	float SprintSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Config|Movement")
	float CrouchSpeed = 200.f;
};
