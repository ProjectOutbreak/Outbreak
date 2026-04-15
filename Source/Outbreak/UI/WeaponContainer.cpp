// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponContainer.h"
#include "Components/TextBlock.h"

void UWeaponContainer::SetAmmoText(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (CurrentAmmoText && TotalAmmoText)
	{
		FString CurrentAmmoStr = FString::Printf(TEXT("%d"), CurrentAmmo);
		FString TotalAmmoStr = FString::Printf(TEXT("%d"), TotalAmmo);
		
		CurrentAmmoText->SetText(FText::FromString(CurrentAmmoStr));
		TotalAmmoText->SetText(FText::FromString(TotalAmmoStr));
	}
}
void UWeaponContainer::SetWeaponIcon(UTexture2D* Icon)
{
	if (IsValid(MainWeaponIcon))
	{
		if (Icon)
		{
			MainWeaponIcon->SetBrushFromTexture(Icon);
			MainWeaponIcon->SetVisibility(ESlateVisibility::Visible);
			CurrentAmmoText->SetVisibility(ESlateVisibility::Visible);
			TotalAmmoText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			MainWeaponIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWeaponContainer::SetSubslotIcon(UTexture2D* Icon, int32 SubslotNum)
{
	if (SubslotNum == 1)
		{
			//2번무기 없어서 일단 공백처리
		}
	else if (SubslotNum == 2)
	{
		SubSlot2 ->SetWeaponIcon(Icon);
	}
}


