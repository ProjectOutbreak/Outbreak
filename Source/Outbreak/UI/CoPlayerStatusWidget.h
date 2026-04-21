// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoPlayerStatusWidget.generated.h"

class UTextBlock;
class UProgressBar;
UCLASS()
class OUTBREAK_API UCoPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* User_Name;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;	
	TWeakObjectPtr<APlayerState> TargetPS;

	APlayerState* GetTargetPS() const { return TargetPS.Get(); }

	void Setup(APlayerState* PS);
	void SetNickname(FString NewName);
	void UpdateHealthBar(float Ratio);
};
