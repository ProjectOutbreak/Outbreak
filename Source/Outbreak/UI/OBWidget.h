// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OBWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;

UCLASS()
class OUTBREAK_API UOBWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
	
	void SetMatchTimeText(float Time);
	void SetCurrentPhaseText(FString Phase);
	void SetAlivePlayerCountText(int32 Count);
	void SetAnnouncementText(FString AnnouncementText);
	void SetTotalZombieKillsText(int32 TotalKills);
	void SetZombieKillsText(int32 Kills);
	void SetAmmoText(int32 currentAmmo, int32 TotalAmmo);
	void SetWeaponTypeText(FString Type);
	void SetCurrentHealth(int32 CurrentHealth, float HealthPercent);
	
	void SetCutsceneMode(bool bEnable);

	
	
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* MiniMapImage;

	UPROPERTY(meta = (BindWidget))
	UImage* AimImage;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchTimeTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PhaseTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AlivePlayerCountTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalZombieKillsTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ZombieKillsTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponTypeTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentHealthTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	//-------------------------//
	UPROPERTY(meta = (BindWidget))
	UButton* BtnLow;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnMedium;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnHigh;

	UFUNCTION() void OnLowClicked();
	UFUNCTION() void OnMediumClicked();
	UFUNCTION() void OnHighClicked();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniMap", meta=(AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniMap", meta=(AllowPrivateAccess = "true"))
	UMaterialInterface* MiniMapMaterial;
	
};
