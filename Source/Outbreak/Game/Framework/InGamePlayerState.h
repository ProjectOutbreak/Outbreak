// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Util/Define.h"
#include "InGamePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAmmoChangedSignature);

UCLASS()
class OUTBREAK_API AInGamePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AInGamePlayerState();

	void AddZombieKill();
	
	int32 GetReserveAmmo(EFirableType Type) const;
	void ConsumeAmmo(EFirableType Type, int32 AmountToConsume);
	void AddAmmo(EFirableType Type, int32 InInAmountToAdd = 0);
	void SetIsDead(bool bDead);

	UFUNCTION(BlueprintCallable)
	bool IsDead() const {return bIsDead;}
	
	FOnPlayerAmmoChangedSignature OnPlayerAmmoChangedDelegate;
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_ZombieKills();
	
	UPROPERTY(ReplicatedUsing = OnRep_ZombieKills)
	int32 ZombieKills;

	UFUNCTION()
	void OnRep_ReserveAmmoArray();
	
	UPROPERTY(ReplicatedUsing = OnRep_ReserveAmmoArray)
	TArray<FAmmoCount> ReserveAmmoArray;

	UPROPERTY()
	TMap<EFirableType, FAmmoData> AmmoDataMap;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> AmmoDataTable;

	UPROPERTY(ReplicatedUsing=OnRep_IsDead, BlueprintReadOnly)
	bool bIsDead;

	UFUNCTION()
	void OnRep_IsDead();
private:
	void InitializeAmmo();
};