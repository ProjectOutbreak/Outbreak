// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphicsSettingsLibrary.h"
#include "GameFramework/GameUserSettings.h"


namespace 
{
	static const TCHAR* CustomSection = TEXT("/Script/Outbreak.Graphics");
	static const TCHAR* InitKey = TEXT("bGraphicsPresetInitialized");
}

static UGameUserSettings* GetSettings()
{
	return (GEngine ? GEngine->GetGameUserSettings() : nullptr);
}

void UGraphicsSettingsLibrary::ApplyPreset(EOBGraphicsPreset Preset, bool bSave)
{
	if (UGameUserSettings* Settings = GetSettings())
	{
		const int32 Level = static_cast<int32>(Preset);
		Settings->SetOverallScalabilityLevel(Level);
		Settings->ApplySettings(false);
		if (bSave)
		{
			Settings->SaveSettings();
		}
	}
}

void UGraphicsSettingsLibrary::ApplyCustom(const FScalabilityPreset& Custom, bool bSave)
{
	if (UGameUserSettings* Settings = GetSettings())
	{
		auto Clamp = [](int32 v){ return FMath::Clamp(v, 0, 4); };
		Settings->SetViewDistanceQuality(Clamp(Custom.ViewDistance));
		Settings->SetAntiAliasingQuality(Clamp(Custom.AntiAliasing));
		Settings->SetPostProcessingQuality(Clamp(Custom.PostProcess));
		Settings->SetShadowQuality(Clamp(Custom.Shadows));
		Settings->SetGlobalIlluminationQuality(Clamp(Custom.GlobalIllumination));
		Settings->SetReflectionQuality(Clamp(Custom.Reflections));
		Settings->SetTextureQuality(Clamp(Custom.Textures));
		Settings->SetVisualEffectQuality(Clamp(Custom.Effects));
		Settings->SetFoliageQuality(Clamp(Custom.Foliage));
		Settings->SetShadingQuality(Clamp(Custom.Shading));

		Settings->ApplySettings(false);
		if (bSave) Settings->SaveSettings();		
	}
}

FScalabilityPreset UGraphicsSettingsLibrary::GetCurrent()
{
	FScalabilityPreset R;
	if (UGameUserSettings* Settings = GetSettings())
	{
		R.ViewDistance = Settings->GetViewDistanceQuality();
		R.AntiAliasing       = Settings->GetAntiAliasingQuality();
		R.PostProcess        = Settings->GetPostProcessingQuality();
		R.Shadows            = Settings->GetShadowQuality();
		R.GlobalIllumination = Settings->GetGlobalIlluminationQuality();
		R.Reflections        = Settings->GetReflectionQuality();
		R.Textures           = Settings->GetTextureQuality();
		R.Effects            = Settings->GetVisualEffectQuality();
		R.Foliage            = Settings->GetFoliageQuality();
		R.Shading            = Settings->GetShadingQuality();
	}
	return R;
}

void UGraphicsSettingsLibrary::ApplyDefaultGraphics()
{
	bool bInitialized = false;
	GConfig->GetBool(CustomSection, InitKey, bInitialized, GGameUserSettingsIni);

	if (!bInitialized)
	{
		ApplyPreset(EOBGraphicsPreset::Medium, true);
		GConfig->SetBool(CustomSection, InitKey, true, GGameUserSettingsIni);
		GConfig->Flush(false, GGameUserSettingsIni);
	}
}