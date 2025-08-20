// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	virtual void OnEquip(TObjectPtr<class ACharacterPlayer> Character) PURE_VIRTUAL(AEquipmentBase::OnEquip, );
	virtual void OnUnequip() PURE_VIRTUAL(AEquipmentBase::OnUnequip, );
	virtual void OnUse() PURE_VIRTUAL(AEquipmentBase::OnUse, );
	virtual bool CanUse() const PURE_VIRTUAL(AEquipmentBase::CanUse, return false;);

	enum class EEquipmentType GetEquipmentType() const { return EquipmentType; }
	int32 GetSlotIndex() const { return SlotIndex; }
	FString GetEquipmentName() const { return EquipmentName; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void PlayAnimation(TObjectPtr<UAnimMontage> Montage);
	void PlaySound(TObjectPtr<USoundCue> Sound);
	void SetMeshVisibility(bool bVisible);

// --------------------
// Variables
// --------------------
	EEquipmentType EquipmentType;
	FString EquipmentName;
	int32 SlotIndex;
	bool bIsInUse;

	UPROPERTY()
	TObjectPtr<UTexture2D> EquipmentIcon;
	
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> EquipmentMesh;

	UPROPERTY()
	TObjectPtr<ACharacterPlayer> OwnerCharacter;
	
};