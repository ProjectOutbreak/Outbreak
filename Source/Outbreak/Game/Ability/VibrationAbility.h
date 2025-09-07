#pragma once
#include "BasePassiveAbility.h"
#include "VibrationAbility.generated.h"

UCLASS()
class OUTBREAK_API UVibrationAbility : public UBasePassiveAbility
{
	GENERATED_BODY()

public:
	UVibrationAbility();
	virtual void OnEquip() override;
	virtual void OnUnequip() override;

private:
	void OnVibrationAbility() const;

private:
	FTimerHandle VibrationTimerHandle;
	float VibrationInterval = 3.0f;
	float VibrationDamage = 5.0f;
	float VibrationIntensity = 3.0f;
	float VibrationRange = 1000.0f;
};
