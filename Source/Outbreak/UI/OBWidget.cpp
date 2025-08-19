// Fill out your copyright notice in the Description page of Project Settings.

#include "OBWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h" 
#include "Components/ProgressBar.h"
#include "Components/ComboBoxString.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Outbreak/Game/Controller/OBPlayerController.h"
#include "Outbreak/Game/Framework/OutBreakGameState.h"
#include "Outbreak/Game/Graphics/GraphicsSettingsLibrary.h"

void UOBWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BtnResume) BtnResume->OnClicked.AddDynamic(this, &UOBWidget::OnResumeClicked);
	if (BtnGraphics) BtnGraphics->OnClicked.AddDynamic(this, &UOBWidget::OnOpenGraphicsClicked);
	
	const FScalabilityPreset Cur = UGraphicsSettingsLibrary::GetCurrent();

	auto InitCombo = [](UComboBoxString* CB)
	{
		if (!CB) return;
		CB->ClearOptions();
		CB->AddOption(TEXT("Low"));
		CB->AddOption(TEXT("Medium"));
		CB->AddOption(TEXT("High"));
		CB->AddOption(TEXT("Epic"));
		CB->AddOption(TEXT("Cinematic"));
	};
	{
		TArray<UComboBoxString*> AllCBs = {
			CBViewDistance,
			CBAntiAliasing,
			CBPostProcess,
			CBShadows,
			CBGlobalIllumination,
			CBReflections,
			CBTextures,
			CBEffects,
			CBFoliage,
			CBShading
		};
		for (UComboBoxString* CB : AllCBs) { InitCombo(CB); }
	}

	auto SetupCombo = [this](UComboBoxString* CB, int32* PendingPtr, int CurVal)
	{
		if (!CB || !PendingPtr) return;
		*PendingPtr = FMath::Clamp(CurVal, 0, 4);
		CB->SetSelectedIndex(*PendingPtr);
	};

	SetupCombo(CBViewDistance,       &PendingViewDistance,       Cur.ViewDistance);
	SetupCombo(CBAntiAliasing,       &PendingAntiAliasing,       Cur.AntiAliasing);
	SetupCombo(CBPostProcess,        &PendingPostProcess,        Cur.PostProcess);
	SetupCombo(CBShadows,            &PendingShadow,             Cur.Shadows);
	SetupCombo(CBGlobalIllumination, &PendingGlobalIllumination, Cur.GlobalIllumination);
	SetupCombo(CBReflections,        &PendingReflections,        Cur.Reflections);
	SetupCombo(CBTextures,           &PendingTextures,           Cur.Textures);
	SetupCombo(CBEffects,            &PendingEffects,            Cur.Effects);
	SetupCombo(CBFoliage,            &PendingFoliage,            Cur.Foliage);
	SetupCombo(CBShading,            &PendingShading,            Cur.Shading);

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
	if (const AOutBreakGameState* GS = GetWorld()->GetGameState<AOutBreakGameState>())
	{
		SetMatchTimeText(GS->GetMatchTime());
		SetCurrentPhaseText(GS->GetCurrentPhase());
	}
}
	
void UOBWidget::SetCutsceneMode(bool bEnable)
{
	ESlateVisibility NewVisibility = bEnable ? ESlateVisibility::Hidden : ESlateVisibility::Visible;

	TArray<UWidget*> WidgetsToToggle = {
		MiniMapImage,
		AimImage,
		MatchTimeTextBlock,
		PhaseTextBlock,
		AlivePlayerCountTextBlock,
		AnnouncementTextBlock,
		TotalZombieKillsTextBlock,
		ZombieKillsTextBlock,
		AmmoTextBlock,
		WeaponTypeTextBlock,
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

void UOBWidget::SetCurrentPhaseText(FString Phase)
{
	if (PhaseTextBlock)
	{
		FString PhaseStr = FString::Printf(TEXT("%s"), *Phase);
		PhaseTextBlock->SetText(FText::FromString(PhaseStr));
	}
}

void UOBWidget::SetAlivePlayerCountText(int32 Count)
{
	if (AlivePlayerCountTextBlock)
	{
		FString CountText = FString::Printf(TEXT("Alive Player : %d"), Count);
		AlivePlayerCountTextBlock->SetText(FText::FromString(CountText));
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

void UOBWidget::SetTotalZombieKillsText(int32 TotalKills)
{
	if (TotalZombieKillsTextBlock)
	{
		FString CountText = FString::Printf(TEXT("Total Kills : %d"), TotalKills);
		TotalZombieKillsTextBlock->SetText(FText::FromString(CountText));
	}
}

void UOBWidget::SetZombieKillsText(int32 Kills)
{
	if (ZombieKillsTextBlock)
	{
		FString CountText = FString::Printf(TEXT("Kills : %d"), Kills);
		ZombieKillsTextBlock->SetText(FText::FromString(CountText));
	}
}

void UOBWidget::SetAmmoText(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (AmmoTextBlock)
	{
		FString AmmoStr = FString::Printf(TEXT("%d / %d"), CurrentAmmo, TotalAmmo);
		AmmoTextBlock->SetText(FText::FromString(AmmoStr));
	}
}

void UOBWidget::SetWeaponTypeText(FString Type)
{
	if (WeaponTypeTextBlock)
	{
		FString TypeStr = FString::Printf(TEXT("%s"), *Type);
		WeaponTypeTextBlock->SetText(FText::FromString(TypeStr));
	}
}

void UOBWidget::SetCurrentHealth(int32 CurrentHealth, float HealthPercent)
{
	UE_LOG(LogTemp, Warning, TEXT("SetCurrentHealth: Health=%d, Percent=%.2f"), CurrentHealth, HealthPercent);

	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercent);
	}	
	if (CurrentHealthTextBlock)
	{
		FString HealthStr = FString::Printf(TEXT("%d / 100"), CurrentHealth);
		CurrentHealthTextBlock->SetText(FText::FromString(HealthStr));
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
		if (AOBPlayerController* OBPC = Cast<AOBPlayerController>(PC))
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
	FScalabilityPreset P = UGraphicsSettingsLibrary::GetCurrent();

	if (CBViewDistance)        P.ViewDistance = PendingViewDistance;
	if (CBAntiAliasing)        P.AntiAliasing = PendingAntiAliasing;
	if (CBPostProcess)         P.PostProcess  = PendingPostProcess;
	if (CBShadows)             P.Shadows = PendingShadow;
	if (CBGlobalIllumination)  P.GlobalIllumination = PendingGlobalIllumination;
	if (CBReflections)         P.Reflections = PendingReflections;
	if (CBTextures)            P.Textures = PendingTextures;
	if (CBEffects)             P.Effects = PendingEffects;
	if (CBFoliage)             P.Foliage = PendingFoliage;
	if (CBShading)             P.Shading = PendingShading;

	UGraphicsSettingsLibrary::ApplyCustom(P, true);
}


void UOBWidget::OnViewDistanceChanged(FString, ESelectInfo::Type)
{
	if (CBViewDistance)
	{
		const int32 Idx = CBViewDistance->GetSelectedIndex();
		if (Idx >= 0) PendingViewDistance = FMath::Clamp(Idx, 0, 4);
	}
}

void UOBWidget::OnAntiAliasingChanged(FString, ESelectInfo::Type)
{
	if (CBAntiAliasing)
	{
		const int32 Idx = CBAntiAliasing->GetSelectedIndex();
		if (Idx >= 0) PendingAntiAliasing = FMath::Clamp(Idx, 0, 4);
	}
}

void UOBWidget::OnPostProcessChanged(FString, ESelectInfo::Type)
{
	if (CBPostProcess)
	{
		const int32 Idx = CBPostProcess->GetSelectedIndex();
		if (Idx >= 0) PendingPostProcess = FMath::Clamp(Idx, 0, 4);
	}
}

void UOBWidget::OnShadowsChanged(FString SelectedItem, ESelectInfo::Type)
{
	if (CBShadows) {
		const int32 Idx = CBShadows->GetSelectedIndex();
		if (Idx >= 0) PendingShadow = FMath::Clamp(Idx, 0, 4);
	}
}

void UOBWidget::OnGlobalIlluminationChanged(FString, ESelectInfo::Type)
{
	if (CBGlobalIllumination)
	{
		const int32 Idx = CBGlobalIllumination->GetSelectedIndex();
		if (Idx >= 0) PendingGlobalIllumination = FMath::Clamp(Idx, 0, 4);
	}
}

void UOBWidget::OnReflectionsChanged(FString, ESelectInfo::Type)
{
	if (CBReflections)
	{
		const int32 Idx = CBReflections->GetSelectedIndex();
		if (Idx >= 0) PendingReflections = FMath::Clamp(Idx, 0, 4);
	}
}

void UOBWidget::OnTexturesChanged(FString, ESelectInfo::Type)
{
	if (CBTextures)
	{
		const int32 Idx = CBTextures->GetSelectedIndex();
		if (Idx >= 0) PendingTextures = FMath::Clamp(Idx, 0, 4);
	}
}

void UOBWidget::OnEffectsChanged(FString, ESelectInfo::Type)
{
	if (CBEffects)
	{
		const int32 Idx = CBEffects->GetSelectedIndex();
		if (Idx >= 0) PendingEffects = FMath::Clamp(Idx, 0, 4);
	}
}

void UOBWidget::OnFoliageChanged(FString, ESelectInfo::Type)
{
	if (CBFoliage)
	{
		const int32 Idx = CBFoliage->GetSelectedIndex();
		if (Idx >= 0) PendingFoliage = FMath::Clamp(Idx, 0, 4);
	}
}

void UOBWidget::OnShadingChanged(FString, ESelectInfo::Type)
{
	if (CBShading)
	{
		const int32 Idx = CBShading->GetSelectedIndex();
		if (Idx >= 0) PendingShading = FMath::Clamp(Idx, 0, 4);
	}
}

void UOBWidget::OnLowClicked()
{
	UGraphicsSettingsLibrary::ApplyPreset(EOBGraphicsPreset::Low, true);
	RefreshGraphicsCombosFromCurrent();
}

void UOBWidget::OnMediumClicked()
{
	UGraphicsSettingsLibrary::ApplyPreset(EOBGraphicsPreset::Medium, true);
	RefreshGraphicsCombosFromCurrent();
}

void UOBWidget::OnHighClicked()
{
	UGraphicsSettingsLibrary::ApplyPreset(EOBGraphicsPreset::High, true);
	RefreshGraphicsCombosFromCurrent();
}

void UOBWidget::RefreshGraphicsCombosFromCurrent()
{
	const FScalabilityPreset Cur = UGraphicsSettingsLibrary::GetCurrent();
	auto SetCombo = [](UComboBoxString* CB, int32& Pending, int CurVal)
	{
		if(!CB) return;
		Pending = FMath::Clamp(CurVal, 0, 4);
		CB->SetSelectedIndex(Pending);
	};
	SetCombo(CBViewDistance,       PendingViewDistance,       Cur.ViewDistance);
	SetCombo(CBAntiAliasing,       PendingAntiAliasing,       Cur.AntiAliasing);
	SetCombo(CBPostProcess,        PendingPostProcess,        Cur.PostProcess);
	SetCombo(CBShadows,            PendingShadow,             Cur.Shadows);
	SetCombo(CBGlobalIllumination, PendingGlobalIllumination, Cur.GlobalIllumination);
	SetCombo(CBReflections,        PendingReflections,        Cur.Reflections);
	SetCombo(CBTextures,           PendingTextures,           Cur.Textures);
	SetCombo(CBEffects,            PendingEffects,            Cur.Effects);
	SetCombo(CBFoliage,            PendingFoliage,            Cur.Foliage);
	SetCombo(CBShading,            PendingShading,            Cur.Shading);
}