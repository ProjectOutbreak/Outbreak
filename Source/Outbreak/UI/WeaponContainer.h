// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubweaponContainer.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "WeaponContainer.generated.h"

/**
 * 
 */
UCLASS()
class OUTBREAK_API UWeaponContainer : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetAmmoText(int32 CurrentAmmo, int32 TotalAmmo);
	void SetWeaponIcon(UTexture2D* Icon);
	void SetSubslotIcon(UTexture2D* Icon, int32 SubslotNum);
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentAmmoText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TotalAmmoText;

	UPROPERTY(meta = (BindWidget))
	UImage* MainWeaponIcon;

	UPROPERTY(meta = (BindWidget))
	USubweaponContainer* SubSlot1;

	UPROPERTY(meta = (BindWidget))
	USubweaponContainer* SubSlot2;
};
