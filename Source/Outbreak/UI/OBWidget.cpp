// Fill out your copyright notice in the Description page of Project Settings.

#include "OBWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h" 
#include "Components/ComboBoxString.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Outbreak/Util/GraphicOptionHelper.h"
#include "Outbreak/Game/Controller/InGamePlayerController.h"
#include "Outbreak/Game/Framework/InGameState.h"
#include "Outbreak/Game/Graphics/GraphicsSettingsLibrary.h"

void UOBWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BtnResume) BtnResume->OnClicked.AddDynamic(this, &UOBWidget::OnResumeClicked);
	if (BtnGraphics) BtnGraphics->OnClicked.AddDynamic(this, &UOBWidget::OnOpenGraphicsClicked);
	
	const FScalabilityPreset Cur = UGraphicsSettingsLibrary::GetCurrent();

	{
		TArray<UComboBoxString*> AllCBs = {
			CBViewDistance, CBAntiAliasing, CBPostProcess, CBShadows,
			CBGlobalIllumination, CBReflections, CBTextures, CBEffects,
			CBFoliage, CBShading
		};
		GraphicOptionHelper::BulkInitQualityOptions(AllCBs);
	}

	GraphicOptionHelper::SetupComboAndPending(CBViewDistance,       PendingViewDistance,       Cur.ViewDistance);
	GraphicOptionHelper::SetupComboAndPending(CBAntiAliasing,       PendingAntiAliasing,       Cur.AntiAliasing);
	GraphicOptionHelper::SetupComboAndPending(CBPostProcess,        PendingPostProcess,        Cur.PostProcess);
	GraphicOptionHelper::SetupComboAndPending(CBShadows,            PendingShadow,             Cur.Shadows);
	GraphicOptionHelper::SetupComboAndPending(CBGlobalIllumination, PendingGlobalIllumination, Cur.GlobalIllumination);
	GraphicOptionHelper::SetupComboAndPending(CBReflections,        PendingReflections,        Cur.Reflections);
	GraphicOptionHelper::SetupComboAndPending(CBTextures,           PendingTextures,           Cur.Textures);
	GraphicOptionHelper::SetupComboAndPending(CBEffects,            PendingEffects,            Cur.Effects);
	GraphicOptionHelper::SetupComboAndPending(CBFoliage,            PendingFoliage,            Cur.Foliage);
	GraphicOptionHelper::SetupComboAndPending(CBShading,            PendingShading,            Cur.Shading);

	if (CBViewDistance)       { CBViewDistance->OnSelectionChanged.AddDynamic(this, &UOBWidget::OnViewDistanceChanged); }
	if (CBAntiAliasing)       { CBAntiAliasing->OnSelectionChanged.AddDynamic(this, &UOBWidget::OnAntiAliasingChanged); }
	if (CBPostProcess)        { CBPostProcess->OnSelectionChanged.AddDynamic(this, &UOBWidget::OnPostProcessChanged); }
	if (CBShadows)            { CBShadows->OnSelectionChanged.AddDynamic(this, &UOBWidget::OnShadowsChanged); }
	if (CBGlobalIllumination) { CBGlobalIllumination->OnSelectionChanged.AddDynamic(this, &UOBWidget::OnGlobalIlluminationChanged); }
	if (CBReflections)        { CBReflections->OnSelectionChanged.AddDynamic(this, &UOBWidget::OnReflectionsChanged); }
	if (CBTextures)           { CBTextures->OnSelectionChanged.AddDynamic(this, &UOBWidget::OnTexturesChanged); }
	if (CBEffects)            { CBEffects->OnSelectionChanged.AddDynamic(this, &UOBWidget::OnEffectsChanged); }
	if (CBFoliage)            { CBFoliage->OnSelectionChanged.AddDynamic(this, &UOBWidget::OnFoliageChanged); }
	if (CBShading)            { CBShading->OnSelectionChanged.AddDynamic(this, &UOBWidget::OnShadingChanged); }
	
	if (BtnApply) BtnApply->OnClicked.AddDynamic(this, &UOBWidget::OnApplyClicked);
	if (BtnBack)  BtnBack->OnClicked.AddDynamic(this, &UOBWidget::OnBackFromGraphics);

	if (WSMain) WSMain->SetActiveWidgetIndex(0);
	if (BtnLow)    BtnLow->OnClicked.AddDynamic(this, &UOBWidget::OnLowClicked);
	if (BtnMedium) BtnMedium->OnClicked.AddDynamic(this, &UOBWidget::OnMediumClicked);
	if (BtnHigh)   BtnHigh->OnClicked.AddDynamic(this, &UOBWidget::OnHighClicked);
	
	if (!GetWorld() || GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		return; 
	}
	if (RenderTarget && MiniMapMaterial && MiniMapImage)
	{
		UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(MiniMapMaterial, this);
		DynMat->SetTextureParameterValue("MinimapTexture", RenderTarget);
		MiniMapImage->SetBrushFromMaterial(DynMat);
	}
}

void UOBWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (const AInGameState* GS = GetWorld()->GetGameState<AInGameState>())
	{
		SetMatchTimeText(GS->GetMatchTime());
	}
}
	
void UOBWidget::SetCutsceneMode(bool bEnable)
{
	ESlateVisibility NewVisibility = bEnable ? ESlateVisibility::Hidden : ESlateVisibility::Visible;

	TArray<UWidget*> WidgetsToToggle = {
		MiniMapImage,
		AimImage,
		MatchTimeTextBlock,
		AnnouncementTextBlock,
		HealthBar,
		CurrentHealthTextBlock
	};
	for (UWidget* Widget : WidgetsToToggle)
	{
		if (Widget)
		{
			Widget->SetVisibility(NewVisibility);
		}
	}
}

//------InGame UI------//

void UOBWidget::SetMatchTimeText(float Time)
{
	if (MatchTimeTextBlock)
	{
		int32 TotalSeconds = FMath::FloorToInt(Time);
		int32 Minutes = TotalSeconds / 60;
		int32 Seconds = TotalSeconds % 60;
		FString TimeStr = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

		MatchTimeTextBlock->SetText(FText::FromString(TimeStr));
	}
	
}


void UOBWidget::SetAnnouncementText(FString AnnouncementText)
{
	if (AnnouncementTextBlock)
	{
		FString AnnouncementMsg = FString::Printf(TEXT("%s"), *AnnouncementText);
		AnnouncementTextBlock->SetText(FText::FromString(AnnouncementMsg));
	}
}


void UOBWidget::SetAmmoText(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (WeaponHUD)
	{
		WeaponHUD->SetAmmoText(CurrentAmmo, TotalAmmo);
	}
}

void UOBWidget::SetCurrentHealth(int32 CurrentHealth, float HealthPercent)
{
	UE_LOG(LogTemp, Warning, TEXT("SetCurrentHealth: Health=%d, Percent=%.2f"), CurrentHealth, HealthPercent);

	if (HealthBar)
	{
		UMaterialInstanceDynamic* DynamicMat = HealthBar->GetDynamicMaterial();

		if (DynamicMat)
		{
			DynamicMat->SetScalarParameterValue(FName("Percent"), HealthPercent);
		}
	}
	if (CurrentHealthTextBlock)
	{
		FString HealthStr = FString::Printf(TEXT("%d"), CurrentHealth);
		CurrentHealthTextBlock->SetText(FText::FromString(HealthStr));
	}
}
void UOBWidget::SetCrouchState(bool IsCrouch)
{
	// 자식 위젯이 유효한지 체크 후 함수 호출
	if (CrouchDisplay)
	{
		CrouchDisplay->SetCrouchImage(IsCrouch);
	}
}
void UOBWidget::SetWeaponContainer(UTexture2D* Icon)
{
	if (WeaponHUD)
	{
		WeaponHUD -> SetWeaponIcon(Icon);
	}
}

void UOBWidget::SetSubWeaponContainer(UTexture2D* Icon, int32 SlotNum)
{
	if (WeaponHUD)
	{
		WeaponHUD -> SetSubslotIcon(Icon, SlotNum);
	}
}

void UOBWidget::SetBottomInvSlot(UTexture2D* Icon, int32 SlotNum)
{
	if (BottomInv)
	{
		BottomInv -> SetInvIcon(Icon,SlotNum);
	}
}

//------Menu UI------//
void UOBWidget::ShowPauseMenu(bool bShow)
{
	if (!WSMain) return;
	WSMain->SetActiveWidgetIndex(bShow ? 1 : 0);
}

void UOBWidget::ShowGraphics(bool bShow)
{
	if (!WSMain) return;
	WSMain->SetActiveWidgetIndex(bShow ? 2 : 1);
}

void UOBWidget::OnResumeClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AInGamePlayerController* OBPC = Cast<AInGamePlayerController>(PC))
		{
			OBPC->TogglePauseMenu();
		}
		PC->SetPause(false);
		PC->SetInputMode(FInputModeGameOnly{});
		PC->bShowMouseCursor = false;
	}
	ShowPauseMenu(false);
}

//------Graphics Setting UI-------//
void UOBWidget::OnOpenGraphicsClicked()
{
	ShowGraphics(true);
}

void UOBWidget::OnBackFromGraphics()
{
	ShowGraphics(false);
}

void UOBWidget::OnApplyClicked()
{
	const FScalabilityPreset Cur = UGraphicsSettingsLibrary::GetCurrent();
	const FScalabilityPreset P = GraphicOptionHelper::BuildPresetFromPending(
		Cur,
		PendingViewDistance, PendingAntiAliasing, PendingPostProcess, PendingShadow,
		PendingGlobalIllumination, PendingReflections, PendingTextures, PendingEffects,
		PendingFoliage, PendingShading,
		CBViewDistance, CBAntiAliasing, CBPostProcess, CBShadows,
		CBGlobalIllumination, CBReflections, CBTextures, CBEffects, CBFoliage, CBShading
	);
	UGraphicsSettingsLibrary::ApplyCustom(P, true);
}

void UOBWidget::OnViewDistanceChanged(FString, ESelectInfo::Type)
{
	GraphicOptionHelper::SetPendingFromCombo(CBViewDistance, PendingViewDistance);
}

void UOBWidget::OnAntiAliasingChanged(FString, ESelectInfo::Type)
{
	GraphicOptionHelper::SetPendingFromCombo(CBAntiAliasing, PendingAntiAliasing);
}

void UOBWidget::OnPostProcessChanged(FString, ESelectInfo::Type)
{
	GraphicOptionHelper::SetPendingFromCombo(CBPostProcess, PendingPostProcess);
}

void UOBWidget::OnShadowsChanged(FString, ESelectInfo::Type)
{
	GraphicOptionHelper::SetPendingFromCombo(CBShadows, PendingShadow);
}

void UOBWidget::OnGlobalIlluminationChanged(FString, ESelectInfo::Type)
{
	GraphicOptionHelper::SetPendingFromCombo(CBGlobalIllumination, PendingGlobalIllumination);
}

void UOBWidget::OnReflectionsChanged(FString, ESelectInfo::Type)
{
	GraphicOptionHelper::SetPendingFromCombo(CBReflections, PendingReflections);
}

void UOBWidget::OnTexturesChanged(FString, ESelectInfo::Type)
{
	GraphicOptionHelper::SetPendingFromCombo(CBTextures, PendingTextures);
}

void UOBWidget::OnEffectsChanged(FString, ESelectInfo::Type)
{
	GraphicOptionHelper::SetPendingFromCombo(CBEffects, PendingEffects);
}

void UOBWidget::OnFoliageChanged(FString, ESelectInfo::Type)
{
	GraphicOptionHelper::SetPendingFromCombo(CBFoliage, PendingFoliage);
}

void UOBWidget::OnShadingChanged(FString, ESelectInfo::Type)
{
	GraphicOptionHelper::SetPendingFromCombo(CBShading, PendingShading);
}

void UOBWidget::OnLowClicked()
{
	GraphicOptionHelper::StagePresetLevel(
		0,
		PendingViewDistance, PendingAntiAliasing, PendingPostProcess, PendingShadow,
		PendingGlobalIllumination, PendingReflections, PendingTextures, PendingEffects,
		PendingFoliage, PendingShading,
		CBViewDistance, CBAntiAliasing, CBPostProcess, CBShadows,
		CBGlobalIllumination, CBReflections, CBTextures, CBEffects, CBFoliage, CBShading
	);
}

void UOBWidget::OnMediumClicked()
{
	GraphicOptionHelper::StagePresetLevel(1,
		PendingViewDistance, PendingAntiAliasing, PendingPostProcess, PendingShadow,
		PendingGlobalIllumination, PendingReflections, PendingTextures, PendingEffects,
		PendingFoliage, PendingShading,
		CBViewDistance, CBAntiAliasing, CBPostProcess, CBShadows,
		CBGlobalIllumination, CBReflections, CBTextures, CBEffects, CBFoliage, CBShading
	);
}

void UOBWidget::OnHighClicked()
{
	GraphicOptionHelper::StagePresetLevel(2,
		PendingViewDistance, PendingAntiAliasing, PendingPostProcess, PendingShadow,
		PendingGlobalIllumination, PendingReflections, PendingTextures, PendingEffects,
		PendingFoliage, PendingShading,
		CBViewDistance, CBAntiAliasing, CBPostProcess, CBShadows,
		CBGlobalIllumination, CBReflections, CBTextures, CBEffects, CBFoliage, CBShading
	);
}