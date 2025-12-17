#pragma once

#include "WeaponBase.h"
#include "Outbreak/Data/GameData.h"
#include "MeleeBase.generated.h"

class ACharacterPlayer;

UCLASS(Abstract)
class OUTBREAK_API AMeleeBase : public AWeaponBase
{
	GENERATED_BODY()
	//----- Functions -----//
public:
	AMeleeBase();
	
	virtual void OnEquip() override;
	virtual void OnUse() override;
	virtual void OnEndUse() override;
	virtual bool CanUse() const override;

	UFUNCTION(BlueprintCallable, Category = "Melee")
	void ResetAttack();
	
	UFUNCTION(BlueprintCallable, Category = "Melee")
	void OnHitDetectionBegin();

	UFUNCTION(BlueprintCallable, Category = "Melee")
	void OnHitDetectionEnd();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void PerformHitDetection();


protected:
	//----- Variables -----//
	FMeleeData MeleeData;

	UPROPERTY(EditDefaultsOnly)
	FName HitStartSocketName = TEXT("Melee_Start");
	UPROPERTY(EditDefaultsOnly)
	FName HitEndSocketName = TEXT("Melee_End");
	
	bool bIsAttacking = false;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActors;
	bool bIsTracing = false;

	UPROPERTY()
	TObjectPtr<ACharacter> CachedOwnerCharacter;
};
