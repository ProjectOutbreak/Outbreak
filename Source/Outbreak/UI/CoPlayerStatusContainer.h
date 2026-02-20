// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoPlayerStatusContainer.generated.h"


class UVerticalBox;
class UCoPlayerStatusWidget;

UCLASS()
class OUTBREAK_API UCoPlayerStatusContainer : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidgetOptional))
	UVerticalBox* VB_List;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCoPlayerStatusWidget> CoPlayerStatusClass;

	void AddPlayer(APlayerState* PS);
	void ClearList();
	void UpdateChildHealth(APlayerState* playerState, float ratio);
};
