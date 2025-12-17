// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentBase.h"

AEquipmentBase::AEquipmentBase()
{
	bReplicates = true;
	AActor::SetReplicateMovement(true);
	
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;
	
	PrimaryActorTick.bCanEverTick = true;
	EquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentMesh"));
	EquipmentMesh->SetupAttachment(RootComponent);
	EquipmentMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	EquipmentMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
}

void AEquipmentBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEquipmentBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 

}