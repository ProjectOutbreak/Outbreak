// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UOBWidget;

UCLASS()
class OUTBREAK_API AInGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	void DisplayAlivePlayerCount(int32 AlivePlayerCount);
	void DisplayAnnouncementMessage(const FString& Message);
	void DisplayTotalZombieKills(int32 TotalZombieKills);
	void DisplayZombieKills(int32 ZombieKills);
	void DisplayAmmo(int32 CurrentAmmo, int32 TotalAmmo);
	void DisplayCurrentHealth(int32 CurrentHealth);
	void SetCutsceneMode(bool bEnable);
	void SetCrouchIcon(bool IsCrouch);
	
private:
	void CreateInGameWidget();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InGameWidgetClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PerformanceWidgetClass;

	UPROPERTY()
	TObjectPtr<UOBWidget> InGameWidgetInstance;
	
public:
	// ~ Begin Getter & Setter
	TObjectPtr<UOBWidget> GetInGameWidget() const { return InGameWidgetInstance;}
	// ~ End Getter & Setter
};