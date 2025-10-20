// Fill out your copyright notice in the Description page of Project Settings.


#include "FootStepComponent.h"
#include "Outbreak/Character/CharacterBase.h"
#include "Outbreak/Manager/SoundManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

UFootStepComponent::UFootStepComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


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
	const FVector TraceStart = FootLocation + FVector(0, 0, 50.0f);
	const FVector TraceEnd   = FootLocation - FVector(0, 0, 50.0f);
    
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(OwningCharacter);

	// 라인 트레이스 실행
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Footstep, QueryParams);

	// --- 디버그 드로잉 코드 시작 ---
	// 트레이스가 무언가에 맞았다면 초록색 선과 히트 지점에 구체를 그림
	if (bHit)
	{
		DrawDebugLine(GetWorld(), TraceStart, HitResult.ImpactPoint, FColor::Green, false, 5.0f, 0, 1.0f);
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.0f, 12, FColor::Green, false, 5.0f);
	}
	// 트레이스가 아무것에도 맞지 않았다면 빨간색 선을 전체 경로에 그림
	else
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5.0f, 0, 1.0f);
	}
	// --- 디버그 드로잉 코드 끝 ---

	if (bHit)
	{
		if (UPhysicalMaterial* PhysMat = HitResult.PhysMaterial.Get())
		{
			const EPhysicalSurface SurfaceType = PhysMat->SurfaceType;
			SoundManager->PlayFootStepSound(SurfaceType, HitResult.ImpactPoint);
		}
	}
}

