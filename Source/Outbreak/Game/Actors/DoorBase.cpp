// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorBase.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Chaos/ChaosEngineInterface.h"

// Sets default values
ADoorBase::ADoorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrameMesh"));
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimelineComp"));

	DoorFrame->SetupAttachment(RootComponent);
	DoorMesh->AttachToComponent(DoorFrame, FAttachmentTransformRules::KeepRelativeTransform);

	DoorDestructibleMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("DoorDestructibleMesh"));
	DoorDestructibleMesh->SetupAttachment(RootComponent);
	DoorDestructibleMesh->SetSimulatePhysics(true);
	DoorDestructibleMesh -> SetVisibility(false);
	DoorDestructibleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌도 비활성화

}

// Called when the game starts or when spawned
void ADoorBase::BeginPlay()
{
	Super::BeginPlay();
	InitialRotation = DoorMesh->GetRelativeRotation();
	if (DoorMesh)
	{
		InitialRotation = DoorMesh->GetRelativeRotation();
	}
    
	if (DoorTimelineComp)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateTimeline"));
		DoorTimelineComp->AddInterpFloat(DoorTimelineFloatCurve, TimelineCallback);
	}	

}

// Called every frame
void ADoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorBase::Interact_Implementation(APawn* InstigatorPawn)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector DoorLocation = GetActorLocation();
		FVector PlayerLocation = InstigatorPawn->GetActorLocation();
		
		FVector DoorForwardVector = GetActorForwardVector();
		DrawDebugLine(World, DoorLocation, DoorLocation + DoorForwardVector * 150.0f, FColor::Red, false, 5.0f, 0, 5.0f);
		
		FVector DirectionToPlayer = (PlayerLocation - DoorLocation).GetSafeNormal();
		DrawDebugLine(World, DoorLocation, DoorLocation + DirectionToPlayer * 150.0f, FColor::Blue, false, 5.0f, 0, 5.0f);
		
		float DotProduct = FVector::DotProduct(DoorForwardVector, DirectionToPlayer);
		FString DebugText = FString::Printf(TEXT("Dot Product: %.2f"), DotProduct);
		DrawDebugString(World, DoorLocation + FVector(0, 0, 100.0f), DebugText, nullptr, FColor::White, 5.0f, true);
	}
		
	if (!DoorMesh || !DoorTimelineComp || !InstigatorPawn)
	{
		return;
	}

	if (CurrentStatus == EDoorStatus::Close)
	{
		TargetRotation = InitialRotation;

		FVector DoorLocation = GetActorLocation();
		FVector PlayerLocation = InstigatorPawn->GetActorLocation();
		FVector DirectionToPlayer = (PlayerLocation - DoorLocation).GetSafeNormal();
		
		FVector DoorForwardVector = DoorMesh->GetRightVector(); 
        
		float DotProduct = FVector::DotProduct(DoorForwardVector, DirectionToPlayer);

		if (DotProduct > 0.0f)
		{
			// 플레이어가 문 앞에 있을 때
			UE_LOG(LogTemp, Display, TEXT("Door Movement: 정면"));
			TargetRotation.Yaw -= 90.0f;
		}
		else
		{
			// 플레이어가 문 뒤에 있을 때
			UE_LOG(LogTemp, Display, TEXT("Door Movement: 반대"));
			TargetRotation.Yaw += 90.0f;
		}
		
		CurrentStatus = EDoorStatus::Open;
		DoorTimelineComp->PlayFromStart();
	}
	else if (CurrentStatus == EDoorStatus::Open)
	{
		CurrentStatus = EDoorStatus::Close;
		DoorTimelineComp->ReverseFromEnd();
	}

	DoorMesh->SetRelativeRotation(TargetRotation);
}

bool ADoorBase::IsOpen() const
{
	return CurrentStatus == EDoorStatus::Open;
}

void ADoorBase::UpdateTimeline(float Output)
{
	FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Output);
	DoorMesh->SetRelativeRotation(NewRotation);
}

void ADoorBase::ApplyChaosDamage(int32 DamageAmount)
{
	// 이미 파괴되었다면 아무것도 하지 않습니다.
	if (HP <= 0)
	{
		return;
	}

	HP -= DamageAmount;

	// 로그를 출력하여 HP가 깎이는 것을 확인합니다.
	UE_LOG(LogTemp, Warning, TEXT("Door HP: %d"), HP);

	// HP가 0 이하가 되면 파괴 로직을 실행합니다.
	if (HP <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Door has been destroyed!"));
		DoorFrame->SetVisibility(false);
		DoorFrame->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DoorDestructibleMesh->SetVisibility(true);
		DoorDestructibleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		
		const FVector ImpulseOrigin = GetActorLocation();
		DoorDestructibleMesh->AddRadialImpulse(ImpulseOrigin, 300.f, 1000000.f, ERadialImpulseFalloff::RIF_Constant, true);
	}
}
