// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "PaperSpriteComponent.h"
#include "Components/TextRenderComponent.h"
#include "Outbreak/Character/CharacterBase.h"
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
	void SetCurrentInteractable(AActor* NewInteractable);
	void ClearCurrentInteractable(AActor* OldInteractable);
	FORCEINLINE TScriptInterface<IInteractInterface> GetCurrentInteractable() const { return CurrentInteractable; }

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

	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> TopViewCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	ECameraMode CurrentCameraMode = ECameraMode::FPS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneCaptureComponent2D* SceneCapture;

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InputMappingContext, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<EPlayerControlType, class UPlayerControlData*> PlayerControlMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeCameraAction;
	
	EPlayerControlType CurrentCharacterControlType;

	// UI & HUD
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<UPaperSpriteComponent> PlayerIconSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<UTextRenderComponent> PlayerNameText;

private:
	TScriptInterface<IInteractInterface> CurrentInteractable;
};
