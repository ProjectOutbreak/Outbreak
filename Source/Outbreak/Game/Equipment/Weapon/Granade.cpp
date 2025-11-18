#include "Granade.h"


AGranade::AGranade()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Game/Art/Equipments/Meshes/Granade/SKM_Granade.SKM_Granade"));
	if (MeshRef.Succeeded())
	{
		EquipmentMesh->SetSkeletalMesh(MeshRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> ThrowableDataTableRef(TEXT("/Game/Data/DT_Throwable.DT_Throwable"));
	if (ThrowableDataTableRef.Succeeded())
	{
		const FString ContextString;
		const FThrowableData* Data = ThrowableDataTableRef.Object->FindRow<FThrowableData>(FName("Granade"), ContextString);
		if (Data)
		{
			ThrowableData = *Data;
			CurrentAmmo = ThrowableData.MaxCount;
		}
		EquipmentType = EEquipmentType::ThrowableWeapon;
	}
}