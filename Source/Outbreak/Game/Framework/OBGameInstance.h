// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Engine/StreamableManager.h"
#include "Outbreak/Util/Define.h"
#include "OBGameInstance.generated.h"

UCLASS()
class OUTBREAK_API UOBGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	//-----Variables-----//
	UPROPERTY()
	TArray<TSoftObjectPtr<UObject>> AssetsToPreload;

	UPROPERTY()
	TSoftClassPtr<UUserWidget> OBWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> CachedWidgetClass = nullptr;
	
protected:
	UPROPERTY()
	ETimePreset SelectedTimePreset = ETimePreset::RandomOne;
public:
	//-----Function-----//
	UFUNCTION()
	TSubclassOf<UUserWidget> GetCachedWidgetClass() const { return CachedWidgetClass; }
	
	virtual void Init() override;

	UFUNCTION()
	void BeginLoading();

	UFUNCTION()
	void OnAssetsLoaded();
	
	UFUNCTION()
	void SetSelectedTimePreset(ETimePreset InPreset) {SelectedTimePreset = InPreset;}

	UFUNCTION()
	ETimePreset GetSelectedTimePreset() const { return SelectedTimePreset; }

	UFUNCTION()
	void ApplySelectedTimePreset(); // 테스트용


private:
	FStreamableManager StreamableManager;	
};
