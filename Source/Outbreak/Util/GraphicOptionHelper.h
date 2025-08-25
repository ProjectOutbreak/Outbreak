#pragma once

#include "Components/ComboBoxString.h"


class GraphicOptionHelper
{
	
public:
	static FORCEINLINE void StagePresetLevel(
		int32 L,
		int32& PendingViewDistance,
		int32& PendingAntiAliasing,
		int32& PendingPostProcess,
		int32& PendingShadow,
		int32& PendingGlobalIllumination,
		int32& PendingReflections,
		int32& PendingTextures,
		int32& PendingEffects,
		int32& PendingFoliage,
		int32& PendingShading,
		UComboBoxString* CBViewDistance,
		UComboBoxString* CBAntiAliasing,
		UComboBoxString* CBPostProcess,
		UComboBoxString* CBShadows,
		UComboBoxString* CBGlobalIllumination,
		UComboBoxString* CBReflections,
		UComboBoxString* CBTextures,
		UComboBoxString* CBEffects,
		UComboBoxString* CBFoliage,
		UComboBoxString* CBShading)
	{
		PendingViewDistance       = L; if (CBViewDistance)       CBViewDistance->SetSelectedIndex(L);
		PendingAntiAliasing       = L; if (CBAntiAliasing)       CBAntiAliasing->SetSelectedIndex(L);
		PendingPostProcess        = L; if (CBPostProcess)        CBPostProcess->SetSelectedIndex(L);
		PendingShadow             = L; if (CBShadows)            CBShadows->SetSelectedIndex(L);
		PendingGlobalIllumination = L; if (CBGlobalIllumination) CBGlobalIllumination->SetSelectedIndex(L);
		PendingReflections        = L; if (CBReflections)        CBReflections->SetSelectedIndex(L);
		PendingTextures           = L; if (CBTextures)           CBTextures->SetSelectedIndex(L);
		PendingEffects            = L; if (CBEffects)            CBEffects->SetSelectedIndex(L);
		PendingFoliage            = L; if (CBFoliage)            CBFoliage->SetSelectedIndex(L);
		PendingShading            = L; if (CBShading)            CBShading->SetSelectedIndex(L);
	}
};
