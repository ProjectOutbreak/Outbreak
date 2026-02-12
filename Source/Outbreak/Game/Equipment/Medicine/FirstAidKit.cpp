#include "FirstAidKit.h"


AFirstAidKit::AFirstAidKit()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Game/Art/Equipments/Meshes/FirstAidKit/SKM_lata_corpo_001.SKM_lata_corpo_001"));
	if (MeshRef.Succeeded())
	{
		EquipmentMesh->SetSkeletalMesh(MeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> MedicineDataTableRef(TEXT("/Game/Data/DT_Medicine.DT_Medicine"));
	if (MedicineDataTableRef.Succeeded())
	{
		const FString ContextString;
		const FMedicineData* Data = MedicineDataTableRef.Object->FindRow<FMedicineData>(FName("FirstAidKit"), ContextString);
		if (Data)
		{
			MedicineData = *Data;
		}
	}
	EquipmentType = EEquipmentType::Medicine;
}
