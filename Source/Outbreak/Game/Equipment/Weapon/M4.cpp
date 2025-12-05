// Fill out your copyright notice in the Description page of Project Settings.


#include "M4.h"

AM4::AM4()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Game/Art/Equipments/Meshes/M4/SKM_M4.SKM_M4"));
	if (MeshRef.Succeeded())
	{
		EquipmentMesh->SetSkeletalMesh(MeshRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> FireAnimRef(TEXT("/Game/Animations/Equipments/M4/A_M4_Fire.A_M4_Fire"));
	if (FireAnimRef.Succeeded())
	{
		FireAnim = FireAnimRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> ReloadAnimRef(TEXT("/Game/Animations/Equipments/M4/A_M4_Reload.A_M4_Reload"));
	if (ReloadAnimRef.Succeeded())
	{
		ReloadAnim = ReloadAnimRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> ReloadEmptyAnimRef(TEXT("/Game/Animations/Equipments/M4/A_M4_ReloadEmpty.A_M4_ReloadEmpty"));
	if (ReloadEmptyAnimRef.Succeeded())
	{
		ReloadEmptyAnim = ReloadEmptyAnimRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundRef(TEXT("/Game/Audio/SFX/Cues/Equipment/SC_Gun_Shot.SC_Gun_Shot"));
	if (FireSoundRef.Succeeded())
	{
		FireSound = FireSoundRef.Object;
	}
	
	// TODO : Temp Load DataTable
	static ConstructorHelpers::FObjectFinder<UDataTable> FirableDataTableRef(TEXT("/Game/Data/DT_Firable.DT_Firable"));
	if (FirableDataTableRef.Succeeded())
	{
		const FString ContextString;
		const FFirableData* Data = FirableDataTableRef.Object->FindRow<FFirableData>(FName("M4"), ContextString);
		if (Data)
		{
			FirableData = *Data;
		}
	}
}

void AM4::BeginPlay()
{
	Super::BeginPlay();
}

void AM4::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
