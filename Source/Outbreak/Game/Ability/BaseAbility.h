// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Util/EnumHelper.h"
#include "BaseAbility.generated.h"

UCLASS()
class OUTBREAK_API UBaseAbility : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(ACharacterBase* InOwner) { Owner = InOwner;}
	virtual EAbilityType GetAbilityType() const { return AbilityType; }
	virtual FString GetAbilityName() const { return EnumHelper::EnumToString(AbilityType); }

protected:
	TObjectPtr<ACharacterBase> GetOwner() const { return Owner; }

protected:
	EAbilityType AbilityType = EAbilityType::None;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ACharacterBase> Owner;
};