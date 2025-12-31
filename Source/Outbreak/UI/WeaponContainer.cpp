// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponContainer.h"
#include "Components/TextBlock.h"

void UWeaponContainer::SetAmmoText(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (CurrentAmmoText && TotalAmmoText)
	{
		// FString::Printf를 이용한 포맷팅 로직
		FString CurrentAmmoStr = FString::Printf(TEXT("%d"), CurrentAmmo);
		FString TotalAmmoStr = FString::Printf(TEXT("%d"), TotalAmmo);
		
		CurrentAmmoText->SetText(FText::FromString(CurrentAmmoStr));
		TotalAmmoText->SetText(FText::FromString(TotalAmmoStr));
	}
}
