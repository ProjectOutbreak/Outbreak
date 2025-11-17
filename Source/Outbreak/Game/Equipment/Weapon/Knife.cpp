#include "Knife.h"


AKnife::AKnife()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Game/Art/Equipments/Meshes/Knife/SKM_Knife.SKM_Knife"));
	if (MeshRef.Succeeded())
	{
		EquipmentMesh->SetSkeletalMesh(MeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> MeleeDataTableRef(TEXT("/Game/Data/DT_Melee.DT_Melee"));
	if (MeleeDataTableRef.Succeeded())
	{
		const FString ContextString;
		const FMeleeData* Data = MeleeDataTableRef.Object->FindRow<FMeleeData>(FName("Knife"), ContextString);
		if (Data)
		{
			MeleeData = *Data;
		}
	}
	EquipmentType = EEquipmentType::SecondaryWeapon;
	
}
