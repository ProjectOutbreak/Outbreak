// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentBase.h"

#include "Components/BoxComponent.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"

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
	EquipmentMesh->SetCollisionProfileName(TEXT("NoCollision"));

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	InteractionBox->SetBoxExtent(FVector(30.f, 30.f, 30.f));
}

void AEquipmentBase::BeginPlay()
{
	Super::BeginPlay();
	SetPhysicsStatus(true);
}

void AEquipmentBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 

}

void AEquipmentBase::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!InteractionBox->IsSimulatingPhysics()) 
	{
		return;
	}
	if (ACharacterPlayer* Player = Cast<ACharacterPlayer>(InstigatorPawn))
	{
		Player->Server_PickupEquipment(this);
	}
}

void AEquipmentBase::SetPhysicsStatus(bool bIsGround)
{
	if (bIsGround)
	{
		InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		InteractionBox->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		InteractionBox->SetSimulatePhysics(true);
		SetActorHiddenInGame(false);
        
		if (EquipmentMesh)
		{
			EquipmentMesh->SetVisibility(true);
		}
	}
	else
	{
		InteractionBox->SetSimulatePhysics(false);
		InteractionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InteractionBox->SetCollisionProfileName(TEXT("NoCollision"));
		// visibility는 여기서 설정하지 않음 (Equip에서 처리)
	}
}


