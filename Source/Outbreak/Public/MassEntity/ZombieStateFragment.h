#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "ZombieStateFragment.generated.h"

enum class EZombieStateType : uint8;

USTRUCT()
struct OUTBREAK_API FZombieStateFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Default constructor */
	FZombieStateFragment();

	UPROPERTY()
	EZombieStateType CurrentState;

	UPROPERTY()
	bool bIsAlert;
};
