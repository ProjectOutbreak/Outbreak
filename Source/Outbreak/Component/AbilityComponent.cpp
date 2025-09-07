#include "AbilityComponent.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Game/Ability/BaseActiveAbility.h"
#include "Outbreak/Game/Ability/BasePassiveAbility.h"
#include "Outbreak/Util/UObjectHelper.h"

UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAbilityComponent::TryUseAbility(const EAbilityType Type) const
{
	if (const TObjectPtr<UBaseActiveAbility> Ability = Cast<UBaseActiveAbility>(GetAbility(Type)))
	{
		if (Ability->CanUseAbility())
		{
			Ability->Activate();
		}
	}
}

void UAbilityComponent::AddAbility(const TObjectPtr<UBaseAbility>& NewAbility)
{
	if (!UObjectHelper::IsUObjectValid(NewAbility))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] NewAbility is not valid."), CURRENT_CONTEXT);
		return;
	}

	const EAbilityType Type = NewAbility->GetAbilityType();

	NewAbility->Initialize(Cast<ACharacterBase>(GetOwner()));
	AbilityArray.Add(NewAbility);
	AbilityMap.Add(Type, NewAbility);

	if (const TObjectPtr<UBasePassiveAbility> Passive = Cast<UBasePassiveAbility>(NewAbility))
	{
		Passive->OnEquip();
	}
}

void UAbilityComponent::RemoveAbility(const EAbilityType Type)
{
	if (const TObjectPtr<UBaseAbility> Ability = GetAbility(Type))
	{
		AbilityArray.Remove(Ability);
		AbilityMap.Remove(Type);
	}
}

void UAbilityComponent::Release()
{
	for (TObjectPtr Ability : AbilityArray)
	{
		if (!UObjectHelper::IsUObjectValid(Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] Ability is not valid."), CURRENT_CONTEXT);
			continue;
		}
		
		if (const TObjectPtr<UBasePassiveAbility> Passive = Cast<UBasePassiveAbility>(Ability))
		{
			Passive->OnUnequip();
		}
	}
	AbilityArray.Empty();
	AbilityMap.Empty();
}

TObjectPtr<UBaseAbility> UAbilityComponent::GetAbility(const EAbilityType Type) const
{
	if (AbilityMap.Contains(Type))
	{
		return AbilityMap[Type];
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] Ability of type %s not found."), CURRENT_CONTEXT, *EnumHelper::EnumToString(Type));
	return nullptr;
}

void UAbilityComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCooldowns(DeltaTime);
	UpdateDurations(DeltaTime);
}

void UAbilityComponent::UpdateCooldowns(const float DeltaTime)
{
	for (TObjectPtr Ability : AbilityArray)
	{
		if (!UObjectHelper::IsUObjectValid(Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] Ability is not valid."), CURRENT_CONTEXT);
			continue;
		}
		
		if (const TObjectPtr<UBaseActiveAbility> Active = Cast<UBaseActiveAbility>(Ability))
		{
			Active->TickCooldown(DeltaTime);
		}
	}
}

void UAbilityComponent::UpdateDurations(const float DeltaTime)
{
	for (TObjectPtr Ability : AbilityArray)
	{
		if (!UObjectHelper::IsUObjectValid(Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] Ability is not valid."), CURRENT_CONTEXT);
			continue;
		}
		
		if (const TObjectPtr<UBaseActiveAbility> Active = Cast<UBaseActiveAbility>(Ability))
		{
			Active->TickDuration(DeltaTime);
		}
	}
}