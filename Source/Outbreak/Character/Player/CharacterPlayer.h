// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Camera/CameraComponent.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Component/EquipmentController.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Util/Define.h"
#include "CharacterPlayer.generated.h"

class AFirstAidKit;
class AGranade;
class AKnife;
class AM4;
class UPostProcessComponent;
class USpringArmComponent;
class UChildActorComponent;

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
	
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostInitializeComponents() override;
	//~ End APawn Interface.

	void ChangePlayerControl();
	void HandleUse() const { EquipmentController->HandleUse(); }
	void HandleEndUse() const { EquipmentController->HandleEndUse(); }
	void HandleReload() const { EquipmentController->HandleReload(); }
	void HandleEquipBySlot(const int32 SlotNumber) const { EquipmentController->EquipBySlot(SlotNumber); }
	void HandleToggleFireMode() const { EquipmentController->HandleToggleFireMode(); }

	void UpdateToxicAuraEffect(float Intensity);
	
protected:
	virtual void BeginPlay() override;
	virtual void InitCharacterData() override;
	virtual void SetupCollision() override;
	virtual void SetupMovement() override;

	virtual void Die() override;
	virtual void OnRep_Die() override;
	virtual void OnRep_CurrentHealth() override;
	virtual void OnRep_Controller() override;
	
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

private:
	void SetPlayerControl(EPlayerControlType InPlayerControlType);
	void SetPlayerControlData(const class UPlayerControlData* InPlayerControlData);
	void ClearInputMappings() const;

	// ~ For Debugging
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	UFUNCTION(Server, Reliable)
	void Server_DebugTakeDamage();

public:
	bool GetIsCutscenePlaying() const { return bIsCutscenePlaying; }
	bool SetIsCutscenePlaying(const bool bInIsCutscenePlaying) { return bIsCutscenePlaying =  bInIsCutscenePlaying; }
	TObjectPtr<UEquipmentController> GetEquipmentController() const { return EquipmentController; }
// --------------------
// Variables
// --------------------
protected:
	// ~ Begin Components
	UPROPERTY(EditAnywhere, Category = "Player|Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, Category = "Player|Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Components")
	TObjectPtr<UEquipmentController> EquipmentController;
	
	UPROPERTY(VisibleAnywhere, Category = "Player|Components")
	TObjectPtr<UPostProcessComponent> PostProcessComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Player|Components")
	TObjectPtr<UChildActorComponent> UIComponent;
	// ~ End Components
	
	UPROPERTY(EditDefaultsOnly, Category = "Player|VFX")
	TObjectPtr<UMaterialInterface> ToxicAuraPostProcessMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player|Control")
	TMap<EPlayerControlType, TObjectPtr<UPlayerControlData>> PlayerControlMap;
	

	// TODO : for test. delete later
	UPROPERTY()
	TObjectPtr<AWeaponBase> SpawnedWeapon;
	UPROPERTY()
	TSubclassOf<AM4> WeaponToSpawn;
	
	UPROPERTY()
    TObjectPtr<AWeaponBase> KnifeWeapon;
    UPROPERTY()
    TSubclassOf<AKnife> KnifeToSpawn;
	
	UPROPERTY()
	TObjectPtr<AWeaponBase> GrenadeWeapon;
	UPROPERTY()
	TSubclassOf<AGranade> GrenadeToSpawn;
	
	UPROPERTY()
	TObjectPtr<AEquipmentBase> HealWeapon;
	UPROPERTY()
	TSubclassOf<AFirstAidKit> HealToSpawn;

private:
	UPROPERTY()
	TObjectPtr<AController> CachedController;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ToxicAuraMID;
	
	UPROPERTY(Replicated)
	FPlayerData PlayerData;
	
	UPROPERTY(Replicated)
	EPlayerType PlayerType = EPlayerType::Player1;
	
	EPlayerControlType CurrentCharacterControlType = EPlayerControlType::FirstPersonView;
	FGenericTeamId TeamId = 0;
	bool bIsCutscenePlaying = false;
};