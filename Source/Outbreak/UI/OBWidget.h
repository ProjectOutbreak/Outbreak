// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BottomInv.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ComboBoxString.h"
#include "Types/SlateEnums.h"
#include "OBCrouchDisplay.h"
#include "WeaponContainer.h"
#include "OBWidget.generated.h"

class UComboBoxString;
class UTextBlock;
class UImage;
class UButton;
class UCoPlayerStatusContainer;

UCLASS()
class OUTBREAK_API UOBWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
	void SetMatchTimeText(float Time);
	void SetAnnouncementText(FString AnnouncementText);
	void SetAmmoText(int32 currentAmmo, int32 TotalAmmo);
	void SetCurrentHealth(int32 CurrentHealth, float HealthPercent);
	void SetCutsceneMode(bool bEnable);
	void SetCrouchState(bool IsCrouch);
	void SetWeaponContainer(UTexture2D* Icon);
	void SetSubWeaponContainer(UTexture2D* Icon, int32 SlotNum);
	void SetBottomInvSlot(UTexture2D* Icon, int32 SlotNum);

	// Container Widget Function
	UFUNCTION()
	void RefreshPlayerList();
	void UpdateMemberHealth(APlayerState* TargetPS, float NewHealthRatio);
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* MiniMapImage;

	UPROPERTY(meta = (BindWidget))
	UImage* AimImage;

	UPROPERTY(meta = (BindWidget))
	UImage* HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchTimeTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentHealthTextBlock;

	UPROPERTY(meta = (BindWidget))
	UOBCrouchDisplay* CrouchDisplay;

	UPROPERTY(meta = (BindWidget))
	UWeaponContainer* WeaponHUD;

	UPROPERTY(meta = (BindWidget))
	UBottomInv* BottomInv;

	// //--------Graphics--------//
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WSMain = nullptr;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnLow;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnMedium;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnHigh;

	UPROPERTY(meta = (BindWidget)) UComboBoxString* CBViewDistance;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* CBAntiAliasing;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* CBPostProcess;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* CBShadows;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* CBGlobalIllumination;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* CBReflections;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* CBTextures;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* CBEffects;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* CBFoliage;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* CBShading;

	UPROPERTY(meta = (BindWidget)) UButton* BtnResume;
	UPROPERTY(meta = (BindWidget)) UButton* BtnGraphics;
	UPROPERTY(meta = (BindWidget)) UButton* BtnBack;
	UPROPERTY(meta = (BindWidget)) UButton* BtnApply;
	
	UPROPERTY(Transient) int32 PendingViewDistance = 1;
	UPROPERTY(Transient) int32 PendingAntiAliasing = 1;
	UPROPERTY(Transient) int32 PendingPostProcess = 1;
	UPROPERTY(Transient) int32 PendingShadow = 1;
	UPROPERTY(Transient) int32 PendingGlobalIllumination = 1;
	UPROPERTY(Transient) int32 PendingReflections = 1;
	UPROPERTY(Transient) int32 PendingTextures = 1;
	UPROPERTY(Transient) int32 PendingEffects = 1;
	UPROPERTY(Transient) int32 PendingFoliage = 1;
	UPROPERTY(Transient) int32 PendingShading = 1;
	
	// Status Container
	UPROPERTY(meta = (BindWidget)) 
	UCoPlayerStatusContainer* PlayerStatusContainer;
	FTimerHandle InitRetryTimer;
public:	
	UFUNCTION() void ShowPauseMenu(bool bShow);
	UFUNCTION() void ShowGraphics(bool bShow);
	UFUNCTION() void OnLowClicked();
	UFUNCTION() void OnMediumClicked();
	UFUNCTION() void OnHighClicked();
	UFUNCTION() void OnApplyClicked();
	UFUNCTION() void OnResumeClicked();
	UFUNCTION() void OnOpenGraphicsClicked();
    UFUNCTION() void OnBackFromGraphics();

	UFUNCTION() void OnViewDistanceChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnAntiAliasingChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnPostProcessChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnShadowsChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnGlobalIlluminationChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnReflectionsChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnTexturesChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnEffectsChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnFoliageChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnShadingChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniMap", meta=(AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MiniMap", meta=(AllowPrivateAccess = "true"))
	UMaterialInterface* MiniMapMaterial;
	
};
