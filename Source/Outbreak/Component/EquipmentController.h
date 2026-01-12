// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Outbreak/Game/Equipment/EquipmentBase.h"
#include "Outbreak/Util/Define.h"
#include "EquipmentController.generated.h"

class AInGameHUD;
class ACharacterPlayer;

UCLASS()
class OUTBREAK_API UEquipmentController : public UActorComponent
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:	
	UEquipmentController();
	void EquipBySlot(int32 SlotNumber);
	void AddEquipment(const TObjectPtr<class AEquipmentBase>& Equipment);
	void UnEquipCurrentEquipment();
	void RemoveEquipment(class AEquipmentBase* ItemToRemove);
	
	void HandleUse();
	void HandleEndUse();
	void HandleReload();
	void HandleToggleFireMode();

	// Getters
	UFUNCTION(BlueprintCallable)
	bool GetIsOnUse() const { return bIsOnUse; }
	
	UFUNCTION(BlueprintCallable)
	bool GetIsReload() const { return bIsReload; }
	
	UFUNCTION(BlueprintCallable)
	bool GetIsSwapOut() const { return bIsSwapOut; }
	
	UFUNCTION(BlueprintCallable)
	bool GetIsSwapIn() const { return bIsSwapIn; }
	
	UFUNCTION(BlueprintCallable)
	EFireType GetCurrentFireType() const { return CurrentFireType; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmoInMag() const { return CurrentAmmoInMag; }

	UFUNCTION(BlueprintPure)
	AEquipmentBase* GetCurrentEquippedItem() const { return CurrentEquippedItem; }
	
	ACharacterPlayer* GetCachedOwner() const { return CachedOwner; }
	AInGameHUD* GetInGameHUD();
	// ~Getters
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	void Equip(const TObjectPtr<class AEquipmentBase>& Equipment);

	UFUNCTION()
	void OnReloadFinished() { bIsReload = false; }
	
	UFUNCTION()
	void HandleAmmoChanged();

	UFUNCTION(Server, Reliable)
	void Server_EquipBySlot(int32 SlotNumber);
	UFUNCTION(Server, Reliable)
	void Server_HandleUse();
	UFUNCTION(Server, Reliable)
	void Server_HandleEndUse();
	UFUNCTION(Server, Reliable)
	void Server_HandleReload();

	UFUNCTION()
	void OnRep_CurrentEquippedType();
	UFUNCTION()
	void OnRep_CurrentEquippedItem();
	UFUNCTION()
	void OnRep_FirstPrimaryWeapon();
	UFUNCTION()
	void OnRep_SecondPrimaryWeapon();
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	UFUNCTION()
	void OnRep_ThrowableWeapon();
	UFUNCTION()
	void OnRep_FirstMedicine();
	UFUNCTION()
	void OnRep_SecondMedicine();

public:
	FORCEINLINE EEquipmentType GetCurrentEquippedType() const { return CurrentEquippedType; }

// --------------------
// Variables
// --------------------
protected:
	UPROPERTY(Transient)
	TObjectPtr<ACharacterPlayer> CachedOwner;
	TWeakObjectPtr<AInGameHUD> CachedHUD;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentEquippedType, BlueprintReadOnly, Category = "Equipment")
	EEquipmentType CurrentEquippedType = EEquipmentType::None;
	
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_CurrentEquippedItem)
	TObjectPtr<class AEquipmentBase> CurrentEquippedItem = nullptr;
	
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_FirstPrimaryWeapon, meta = (ToolTip = "1"))
	TObjectPtr<class AFirableBase> FirstPrimaryWeapon = nullptr;
	
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_SecondPrimaryWeapon, meta = (ToolTip = "2"))
	TObjectPtr<class AFirableBase> SecondPrimaryWeapon = nullptr;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_SecondaryWeapon, meta = (ToolTip = "3"))
	TObjectPtr<class AWeaponBase> SecondaryWeapon = nullptr;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_ThrowableWeapon, meta = (ToolTip = "4"))
	TObjectPtr<class AThrowableBase> ThrowableWeapon = nullptr;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_FirstMedicine, meta = (ToolTip = "5"))
	TObjectPtr<class AMedicineBase> FirstMedicine = nullptr;
	
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_SecondMedicine, meta = (ToolTip = "6"))
	TObjectPtr<class AMedicineBase> SecondMedicine = nullptr;

private:
	UPROPERTY(Replicated)
	bool bIsReload = false;
	bool bIsOnUse = false;
	bool bIsSwapOut = false;
	bool bIsSwapIn = false;
	EFireType CurrentFireType = EFireType::None;
	int32 CurrentAmmoInMag = 0;
};