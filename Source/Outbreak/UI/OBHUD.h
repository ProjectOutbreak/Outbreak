// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Outbreak/Data/GameData.h"
#include "OBHUD.generated.h"

UCLASS()
class OUTBREAK_API AOBHUD : public AHUD
{
	GENERATED_BODY()


public:
	AOBHUD();
	virtual void BeginPlay() override;
	
	void DisplayAlivePlayerCount(int32 AlivePlayerCount);
	void DisplayAnnouncementMessage(const FString& Message);
	void DisplayTotalZombieKills(int32 TotalZombieKills);
	void DisplayZombieKills(int32 ZombieKills);
	void DisplayAmmo(int32 CurrentAmmo, int32 TotalAmmo);
	void DisplayWeaponType(FString Type);
	void DisplayCurrentHealth(int32 CurrentHealth);
	
	void SetCutsceneMode(bool bEnable);
protected:
	UPROPERTY()
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY()
	FPlayerData PlayerData;
private:
	UPROPERTY()
	class UOBWidget* OB_Widget;
};
