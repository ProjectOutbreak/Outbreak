// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentBase.h"

AEquipmentBase::AEquipmentBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEquipmentBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEquipmentBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEquipmentBase::PlayAnimation(TObjectPtr<UAnimMontage> Montage)
{
}

void AEquipmentBase::PlaySound(TObjectPtr<USoundCue> Sound)
{
}

void AEquipmentBase::SetMeshVisibility(bool bVisible)
{
}

