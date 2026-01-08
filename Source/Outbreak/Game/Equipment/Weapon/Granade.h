#pragma once


#include "CoreMinimal.h"
#include "Outbreak/Game/Equipment/Weapon/ThrowableBase.h"
#include "Granade.generated.h"

UCLASS()
class OUTBREAK_API AGranade : public AThrowableBase
{
	GENERATED_BODY()
	
public:
	AGranade();
	virtual UTexture2D* GetEquipmentIcon() const override;	
};
