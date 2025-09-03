// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Outbreak/Util/Define.h"
#include "EquipmentBase.generated.h"

UCLASS(Abstract)
class OUTBREAK_API AEquipmentBase : public AActor
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:	
	AEquipmentBase();
	virtual void OnEquip() PURE_VIRTUAL(AEquipmentBase::OnEquip, );
	virtual void OnUse() PURE_VIRTUAL(AEquipmentBase::OnUse, );
	virtual void OnEndUse() PURE_VIRTUAL(AEquipmentBase::OnEndUse, );
	virtual bool CanUse() const PURE_VIRTUAL(AEquipmentBase::CanUse, return true;);

	EEquipmentType GetEquipmentType() const { return EquipmentType; }
	int32 GetSlotIndex() const { return SlotIndex; }
	FString GetEquipmentName() const { return EquipmentName; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

// --------------------
// Variables
// --------------------
protected:
	UPROPERTY(EditAnywhere)
	EEquipmentType EquipmentType = EEquipmentType::PrimaryWeapon;
	
	FString EquipmentName = "Equipment";
	int32 SlotIndex = 1;
	bool bIsInUse = false;

	UPROPERTY()
	TObjectPtr<UTexture2D> EquipmentIcon;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> EquipmentMesh;

};