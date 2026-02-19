// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "SubweaponContainer.generated.h"

/**
 * 
 */
UCLASS()
class OUTBREAK_API USubweaponContainer : public UUserWidget
{
	GENERATED_BODY()

	public:
	void SetAmmoText(int32 CurrentAmmo, int32 TotalAmmo);
	void SetWeaponIcon(UTexture2D* Icon);
protected:

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponIcon;

	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* AmmoText;

};
