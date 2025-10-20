// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Engine/StreamableManager.h"
#include "Outbreak/Util/Define.h"
#include "OutBreakGameInstance.generated.h"

UCLASS()
class OUTBREAK_API UOutBreakGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	//-----Variables-----//
	TArray<TSoftObjectPtr<UClass>> ClassesToPreload;
	
	UPROPERTY()
	TSubclassOf<UUserWidget> CachedWidgetClass = nullptr;
	
protected:
	UPROPERTY()
	ETimePreset SelectedTimePreset = ETimePreset::RandomOne;
public:
	//-----Function-----//
	virtual void Init() override;
	
	UFUNCTION()	void AddAssetsPath();
	UFUNCTION()	void BeginLoading();
	UFUNCTION()	void OnAssetsLoaded();
	UFUNCTION()	TSubclassOf<UUserWidget> GetCachedWidgetClass() const { return CachedWidgetClass; }
	UFUNCTION()	void SetSelectedTimePreset(ETimePreset InPreset) {SelectedTimePreset = InPreset;}
	UFUNCTION()	ETimePreset GetSelectedTimePreset() const { return SelectedTimePreset; }
	UFUNCTION()	void ApplySelectedTimePreset(); // 테스트용

private:
	FStreamableManager StreamableManager;	
};
