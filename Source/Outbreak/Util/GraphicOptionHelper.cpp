#include "GraphicOptionHelper.h"
#include "Outbreak/Game/Graphics/GraphicsSettingsLibrary.h"


void GraphicOptionHelper::InitQualityOptions(UComboBoxString* CB)
{
	if (!CB) return;
	CB->ClearOptions();
	CB->AddOption(TEXT("Low"));
	CB->AddOption(TEXT("Medium"));
	CB->AddOption(TEXT("High"));
	CB->AddOption(TEXT("Epic"));
	CB->AddOption(TEXT("Cinematic"));
}

void GraphicOptionHelper::BulkInitQualityOptions(const TArray<UComboBoxString*>& ComboArray)
{
	for (UComboBoxString* CB : ComboArray)
	{
		InitQualityOptions(CB);
	}
}

void GraphicOptionHelper::SetupComboAndPending(UComboBoxString* CB, int32& PendingRef, int32 CurVal)
{
	PendingRef = FMath::Clamp(CurVal, 0, 4);
	if (CB) CB->SetSelectedIndex(PendingRef);
}

void GraphicOptionHelper::SetPendingFromCombo(UComboBoxString* CB, int32& PendingRef)
{
	if (!CB) return;
	const int32 Idx = CB->GetSelectedIndex();
	if (Idx >= 0)
	{
		PendingRef = FMath::Clamp(Idx, 0, 4);
	}
}

FScalabilityPreset GraphicOptionHelper::BuildPresetFromPending(
	const FScalabilityPreset& Cur,
	int32 PendingViewDistance, int32 PendingAntiAliasing, int32 PendingPostProcess,
	int32 PendingShadow, int32 PendingGlobalIllumination, int32 PendingReflections,
	int32 PendingTextures, int32 PendingEffects, int32 PendingFoliage, int32 PendingShading,
	UComboBoxString* CBViewDistance, UComboBoxString* CBAntiAliasing, UComboBoxString* CBPostProcess,
	UComboBoxString* CBShadows, UComboBoxString* CBGlobalIllumination, UComboBoxString* CBReflections,
	UComboBoxString* CBTextures, UComboBoxString* CBEffects, UComboBoxString* CBFoliage, UComboBoxString* CBShading)
{
	FScalabilityPreset P = Cur;

	if (CBViewDistance)       P.ViewDistance       = FMath::Clamp(PendingViewDistance,       0, 4);
	if (CBAntiAliasing)       P.AntiAliasing       = FMath::Clamp(PendingAntiAliasing,       0, 4);
	if (CBPostProcess)        P.PostProcess        = FMath::Clamp(PendingPostProcess,        0, 4);
	if (CBShadows)            P.Shadows            = FMath::Clamp(PendingShadow,             0, 4);
	if (CBGlobalIllumination) P.GlobalIllumination = FMath::Clamp(PendingGlobalIllumination, 0, 4);
	if (CBReflections)        P.Reflections        = FMath::Clamp(PendingReflections,        0, 4);
	if (CBTextures)           P.Textures           = FMath::Clamp(PendingTextures,           0, 4);
	if (CBEffects)            P.Effects            = FMath::Clamp(PendingEffects,            0, 4);
	if (CBFoliage)            P.Foliage            = FMath::Clamp(PendingFoliage,            0, 4);
	if (CBShading)            P.Shading            = FMath::Clamp(PendingShading,            0, 4);

	return P;
}