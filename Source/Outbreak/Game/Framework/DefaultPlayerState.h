// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Util/Define.h"
#include "DefaultPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAmmoChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerInventoryChanged);

UCLASS()
class OUTBREAK_API ADefaultPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ADefaultPlayerState();
	virtual void OnRep_PlayerName() override;

	void AddZombieKill();
	
	int32 GetReserveAmmo(EFirableType Type) const;
	void ConsumeAmmo(EFirableType Type, int32 AmountToConsume);
	void AddAmmo(EFirableType Type, int32 InInAmountToAdd = 0);

	int32 GetThrowableCount(EThrowableType Type) const;
	void AddThrowable(EThrowableType Type, int32 Amount);
	bool ConsumeThrowable(EThrowableType Type, int32 Amount = 1);

	int32 GetMedicineCount(EMedicineType Type) const;
	void AddMedicine(EMedicineType Type, int32 Amount);
	bool ConsumeMedicine(EMedicineType Type, int32 Amount = 1);

	FOnPlayerAmmoChangedSignature OnPlayerAmmoChangedDelegate;
	FOnPlayerInventoryChanged OnInventoryChangedDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Inventory();
	
	UFUNCTION()
	void OnRep_ZombieKills();
	
	UPROPERTY(ReplicatedUsing = OnRep_ZombieKills)
	int32 ZombieKills;

	UFUNCTION()
	void OnRep_ReserveAmmoArray();
	
	UPROPERTY(ReplicatedUsing = OnRep_ReserveAmmoArray)
	TArray<FAmmoCount> ReserveAmmoArray;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<FThrowableCount> ThrowableInventory;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<FMedicineCount> MedicineInventory;

	UPROPERTY()
	TMap<EFirableType, FAmmoData> AmmoDataMap;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> AmmoDataTable;

private:
	void InitializeAmmo();
};