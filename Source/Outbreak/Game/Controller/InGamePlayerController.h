// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
class IInteractInterface;
#include "InGamePlayerController.generated.h"

UCLASS()
class OUTBREAK_API AInGamePlayerController : public APlayerController
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:
	AInGamePlayerController();
	
	void TogglePauseMenu();
	void QuitGame();
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void AcknowledgePossession(class APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	void PerformInteract();

private:
	void FirstPersonMove(const struct FInputActionValue& Value);
	void FirstPersonLook(const struct FInputActionValue& Value);
	void TopMove(const struct FInputActionValue& Value);
	void Jump();
	void StopJumping();
	void Run();
	void StopRun();
	void Crouch();
	void StopCrouch();
	void ChangePlayerControl();
	void GetInteractableObject();

	void Use();
	void EndUse();
	void Reload();
	void SelectEquipment(const struct FInputActionValue& Value);
	void ToggleFireMode();

	UFUNCTION(Server, Reliable)
	void Server_QuitGame();
	// --------------------
// Variables
// --------------------
	UPROPERTY()
	TObjectPtr<class ACharacterPlayer> ControlledCharacter;
	
	UPROPERTY()
	TObjectPtr<class UInputAction> TopMoveAction;
	
	UPROPERTY()
	TObjectPtr<class UInputAction> FirstPersonMoveAction;

	UPROPERTY()
	TObjectPtr<class UInputAction> FirstPersonLookAction;

	UPROPERTY()
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY()
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY()
	TObjectPtr<class UInputAction> CrouchAction;

	UPROPERTY()
	TObjectPtr<class UInputAction> ChangePlayerControlAction;

	UPROPERTY()
	TObjectPtr<class UInputAction> UseAction;
	
	UPROPERTY()
	TObjectPtr<class UInputAction> EndUseAction;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputAction> InteractAction;
	


	UPROPERTY()
	TObjectPtr<class UInputAction> ReloadAction;

	UPROPERTY()
	TObjectPtr<class UInputAction> SelectEquipmentAction;
	
	UPROPERTY()
	TObjectPtr<class UInputAction> ToggleFireModeAction;
	

	UPROPERTY()
	TObjectPtr<class UInputAction> ToggleMenuAction;
	

	UPROPERTY(EditAnywhere, Category = "Config|Movement")
	float WalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Config|Movement")
	float SprintSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Config|Movement")
	float CrouchSpeed = 200.f;
	
	bool bMenuOpen = false;
	
	TScriptInterface<IInteractInterface> FocusedInteractable;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionDistance;
};
