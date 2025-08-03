// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "PaperSpriteComponent.h"
#include "Components/TextRenderComponent.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Game/Gear/Weapon/MainWeapon.h"
#include "Outbreak/Game/Gear/Weapon/WeaponBase.h"
#include "Outbreak/Util/Define.h"
#include "CharacterPlayer.generated.h"

UCLASS()
class OUTBREAK_API ACharacterPlayer : public ACharacterBase, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:
	ACharacterPlayer();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsCrouching() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsSprinting() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsShooting() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetFireMode() const;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsReloading() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitCharacterData() override;
	virtual void SetupCollision() override;
	virtual void SetupMovement() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void OnRep_Die() override;
	
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	void SetCharacterControl(EPlayerControlType NewCharacterControlType);
	void ToggleCameraMode();
	
	void OnFirePressed();
	void OnFireReleased();
	void OnToggleFireMode();
	void ChangeArm();
	void OnReload();
	

	// Movement
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_SetSprinting(bool bNewSprinting);

	UFUNCTION()
	void StartSprinting();

	UFUNCTION()
	void StopSprinting();

	UFUNCTION()
	void BeginCrouch();

	UFUNCTION()
	void EndCrouch();

	void SwapToSlot(EInventorySlotType InSlotType);
	
	UFUNCTION()
	void OnPressedSlot1();

	UFUNCTION()
	void OnPressedSlot2();

private:
	UFUNCTION(Server, Reliable)
	void Server_ChangeArm(EInventorySlotType NewSlot);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ChangeArm(EInventorySlotType NewSlot);

// --------------------
// Variables
// --------------------
public:
	UPROPERTY()
	bool bIsCutscenePlaying = false;
	
protected:
	UPROPERTY(Replicated)
	FPlayerData PlayerData;
	
	UPROPERTY(Replicated)
	EPlayerType PlayerType = EPlayerType::Player1;
	
	FGenericTeamId TeamId = 0;

	// Weapon
	TSubclassOf<AMainWeapon> WeaponClass;
	
	UPROPERTY(Replicated)
	AWeaponBase* CurrentWeapon;
	
	bool bIsAutoFire = false;

	USkeletalMesh* SMGMesh;
	USkeletalMesh* ARMesh;

	// Inventory
	UPROPERTY(Replicated)
	TArray<TSubclassOf<AWeaponBase>> WeaponInventory;
	
	int32 CurrentSlotIndex;
	
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AWeaponBase>> WeaponInstances;
	
	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> TopViewCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	ECameraMode CurrentCameraMode = ECameraMode::FPS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneCaptureComponent2D* SceneCapture;

	// Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> GunMesh;

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InputMappingContext, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<EPlayerControlType, class UPlayerControlData*> PlayerControlMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeCameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeFireModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SwapSlot1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SwapSlot2;
	
	EPlayerControlType CurrentCharacterControlType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed = 200.f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsCrouching = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsShooting = false;

	// UI & HUD
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<UPaperSpriteComponent> PlayerIconSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<UTextRenderComponent> PlayerNameText;
};
