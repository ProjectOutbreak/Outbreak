#pragma once
#include "CoreMinimal.h"
#include "Outbreak/Game/Equipment/Medicine/MedicineBase.h"
#include "FirstAidKit.generated.h"


UCLASS()
class OUTBREAK_API AFirstAidKit : public AMedicineBase
{
	GENERATED_BODY()


public:
	AFirstAidKit();
	
protected:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
};
