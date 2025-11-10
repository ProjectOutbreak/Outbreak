// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Camera/CameraComponent.h"
#include "PaperSpriteComponent.h"
#include "Components/TextRenderComponent.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Component/EquipmentController.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Game/Interface/InteractInterface.h"
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

	virtual void PostInitializeComponents() override;

	void ChangePlayerControl();
	void HandleUse() const { EquipmentController->HandleUse(); }
	void HandleEndUse() const { EquipmentController->HandleEndUse(); }
	void HandleReload() const { EquipmentController->HandleReload(); }
	void HandleEquipBySlot(const int32 SlotNumber) const { EquipmentController->EquipBySlot(SlotNumber); }
	void HandleToggleFireMode() const { EquipmentController->HandleToggleFireMode(); }
	TObjectPtr<class AInGameHUD> GetHud() const { return CachedHUD; }

	void UpdateToxicAuraEffect(float Intensity);
	
protected:
	virtual void BeginPlay() override;
	virtual void InitCharacterData() override;
	virtual void SetupCollision() override;
	virtual void SetupMovement() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void OnRep_Die() override;
	
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

private:
	void SetPlayerControl(EPlayerControlType InPlayerControlType);
	void SetPlayerControlData(const class UPlayerControlData* InPlayerControlData);

// --------------------
// Variables
// --------------------
public:
	UPROPERTY()
	bool bIsCutscenePlaying = false;
	
protected:
	UPROPERTY()
	TObjectPtr<class AInGamePlayerController> CachedPC;
	
	UPROPERTY()
	TObjectPtr<class AInGameHUD> CachedHUD;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UEquipmentController> EquipmentController;
	
	UPROPERTY(Replicated)
	FPlayerData PlayerData;
	
	UPROPERTY(Replicated)
	EPlayerType PlayerType = EPlayerType::Player1;
	
	FGenericTeamId TeamId = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneCaptureComponent2D* SceneCapture;

	// Input
	UPROPERTY()
	TMap<EPlayerControlType, TObjectPtr<class UPlayerControlData>> PlayerControlMap;
	
	EPlayerControlType CurrentCharacterControlType;

	// UI & HUD
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<UPaperSpriteComponent> PlayerIconSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<UTextRenderComponent> PlayerNameText;

	// VFX
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UPostProcessComponent> PostProcessComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> ToxicAuraPostProcessMaterial;

	// TODO : for test. delete later
	UPROPERTY()
	TObjectPtr<class AWeaponBase> SpawnedWeapon;
	UPROPERTY()
	TSubclassOf<class AM4> WeaponToSpawn;

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ToxicAuraMID;

};
