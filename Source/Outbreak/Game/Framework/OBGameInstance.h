// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Engine/StreamableManager.h"
#include "OBGameInstance.generated.h"

UCLASS()
class OUTBREAK_API UOBGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<TSoftObjectPtr<UObject>> AssetsToPreload;

	UPROPERTY()
	TSoftClassPtr<UUserWidget> OBWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> CachedWidgetClass = nullptr;

	UFUNCTION()
	TSubclassOf<UUserWidget> GetCachedWidgetClass() const { return CachedWidgetClass; }
	
	virtual void Init() override;

	UFUNCTION()
	void BeginLoading();

	UFUNCTION()
	void OnAssetsLoaded();
	
private:
	FStreamableManager StreamableManager;	
};
