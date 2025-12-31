// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OBCrouchDisplay.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class OUTBREAK_API UOBCrouchDisplay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCrouchImage(bool IsCrouch);
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* CrouchImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Config")
	UTexture2D* CrouchIconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Config")
	UTexture2D* StandIconTexture;
};
