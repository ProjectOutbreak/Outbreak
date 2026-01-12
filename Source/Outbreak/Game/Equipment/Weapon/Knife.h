#pragma once

#include "MeleeBase.h"
#include "Knife.generated.h"


UCLASS()
class OUTBREAK_API AKnife : public AMeleeBase
{
	GENERATED_BODY()
	
public:
	AKnife();
	virtual UTexture2D* GetEquipmentIcon() const override;
};
