// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentAmmoText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TotalAmmoText;
	
};
