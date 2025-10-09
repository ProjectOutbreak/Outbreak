// Fill out your copyright notice in the Description page of Project Settings.


#include "FootStepComponent.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Manager/SoundManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UFootStepComponent::UFootStepComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UFootStepComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacterBase>(GetOwner());

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		SoundManager = GI->GetSubsystem<USoundManager>();
	}
	
}


void UFootStepComponent::HandleFootStep(const FName& SocketName)
{
	if (!OwningCharacter || !SoundManager) return;

	const FVector FootLocation = OwningCharacter->GetMesh()->GetSocketLocation(SocketName);
	const FVector TraceStart = FootLocation + FVector(0, 0 , 50.0f);
	const FVector TraceEnd = FootLocation + FVector(0, 0, 100.0f);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(OwningCharacter);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		if (UPhysicalMaterial* PhysMat = HitResult.PhysMaterial.Get())
		{
			const EPhysicalSurface SurfaceType = PhysMat->SurfaceType;
			SoundManager->PlayFootStepSound(SurfaceType, HitResult.ImpactPoint);
		}
	}
}


