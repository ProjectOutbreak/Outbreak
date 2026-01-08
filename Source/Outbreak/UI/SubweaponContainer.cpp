// Fill out your copyright notice in the Description page of Project Settings.


#include "SubweaponContainer.h"

void USubweaponContainer::SetWeaponIcon(UTexture2D* Icon)
{
	if (IsValid(WeaponIcon))
	{
		if (Icon)
		{
			WeaponIcon->SetBrushFromTexture(Icon);
			WeaponIcon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			WeaponIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
