// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"
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
	TArray<TSoftObjectPtr<UClass>> ClassesToPreload;
	TArray<TSoftObjectPtr<USoundBase>> SoundsToPreload;
	
	UPROPERTY()	TSubclassOf<UUserWidget> CachedWidgetClass = nullptr;
	UPROPERTY() TArray<TObjectPtr<USoundBase>> CachedBgmSounds;
	TMap<EPhysicalSurface, TObjectPtr<USoundBase>> CachedFootstepSounds;
	
protected:
	UPROPERTY()
	ETimePreset SelectedTimePreset = ETimePreset::RandomOne;
	
private:
	FStreamableManager StreamableManager;

public:
	//-----Function-----//
	virtual void Init() override;
	
	UFUNCTION()	void AddAssetsPath();
	UFUNCTION()	void BeginLoading();
	UFUNCTION()	void OnAssetsLoaded();
	UFUNCTION()	void SetSelectedTimePreset(ETimePreset InPreset) {SelectedTimePreset = InPreset;}
	UFUNCTION()	ETimePreset GetSelectedTimePreset() const { return SelectedTimePreset; }
	UFUNCTION()	void ApplySelectedTimePreset(); // 테스트용

	UFUNCTION()	TSubclassOf<UUserWidget> GetCachedWidgetClass() const { return CachedWidgetClass; }
	const TArray<TObjectPtr<USoundBase>>& GetCachedBgmList() const { return CachedBgmSounds; }
	const TMap<EPhysicalSurface, TObjectPtr<USoundBase>>& GetCachedFootStepSounds() const { return CachedFootstepSounds; }
};
