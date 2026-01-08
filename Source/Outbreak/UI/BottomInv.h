// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "BottomInv.generated.h"

/**
 * 
 */
UCLASS()
class OUTBREAK_API UBottomInv : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* BottomInvSlot1;
	
	UPROPERTY(meta = (BindWidget))
	UImage* BottomInvSlot3;

	UPROPERTY(meta = (BindWidget))
	UImage* BottomInvSlot2;

public:
	void SetInvIcon(UTexture2D* Icon, int32 SlotNum);
};
