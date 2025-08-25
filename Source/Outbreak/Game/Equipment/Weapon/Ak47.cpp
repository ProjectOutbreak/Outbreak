// Fill out your copyright notice in the Description page of Project Settings.

#include "Ak47.h"

AAk47::AAk47()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Game/Art/Equipments/Meshes/SKM_Ak47.SKM_Ak47"));
	if (MeshRef.Succeeded())
	{
		EquipmentMesh->SetSkeletalMesh(MeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Animations/Equipments/ABP_Ak47.ABP_Ak47_C"));
	if (AnimInstanceClassRef.Class)
	{
		EquipmentMesh->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
	// TODO : Temp Load DataTable
	static ConstructorHelpers::FObjectFinder<UDataTable> FirableDataTableRef(TEXT("/Game/Data/DT_Firable.DT_Firable"));
	if (FirableDataTableRef.Succeeded())
	{
		const FString ContextString;
		const FFirableData* Data = FirableDataTableRef.Object->FindRow<FFirableData>(FName("Ak47"), ContextString);
		if (Data)
		{
			FirableData = *Data;
		}
	}
}

void AAk47::BeginPlay()
{
	Super::BeginPlay();
}

void AAk47::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}