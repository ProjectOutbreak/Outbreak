// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Game/Equipment/EquipmentBase.h"
#include "MedicineBase.generated.h"

UCLASS()
class OUTBREAK_API AMedicineBase : public AEquipmentBase
{
	GENERATED_BODY()

// --------------------
// Functions
// --------------------
public:	
	virtual bool CanUse() const override;
	virtual void OnUse() override;
	
	virtual void UseSelf();
	virtual void UseOnTarget(TObjectPtr<class ACharacterPlayer> TargetCharacter);

protected:
	UFUNCTION()
	void OnUseComplete();

	virtual void ApplyHealEffect(TObjectPtr<class ACharacterPlayer> TargetCharacter);
	
// --------------------
// Variables
// --------------------
protected:
	EMedicineType MedicineType;
	FMedicineData MedicineData;
	bool bIsUsing = false;

	FTimerHandle UseTimer;
};
