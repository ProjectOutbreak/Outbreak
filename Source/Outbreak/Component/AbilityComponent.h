#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Outbreak/Game/Ability/BaseAbility.h"
#include "AbilityComponent.generated.h"

UCLASS()
class OUTBREAK_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAbilityComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TryActivateAbility(EAbilityType Type) const;

	void AddAbility(const TObjectPtr<class UBaseAbility>& NewAbility);
	void RemoveAbility(EAbilityType Type);
	void Release();

	TObjectPtr<UBaseAbility> GetAbility(EAbilityType Type) const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UBaseAbility>> AbilityArray;
	
	UPROPERTY(VisibleAnywhere)
	TMap<EAbilityType, TObjectPtr<UBaseAbility>> AbilityMap;

	void UpdateCooldowns(float DeltaTime);
	void UpdateDurations(float DeltaTime);
};
