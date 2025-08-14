// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorBase.h"
#include "Components/BoxComponent.h"

// Sets default values
ADoorBase::ADoorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrameMesh"));
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimelineComp"));
	DoorProxVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorProximityVolume"));

	DoorFrame->SetupAttachment(RootComponent);
	DoorMesh->AttachToComponent(DoorFrame, FAttachmentTransformRules::KeepRelativeTransform);
	DoorProxVolume->AttachToComponent(DoorFrame, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ADoorBase::BeginPlay()
{
	Super::BeginPlay();

	UpdateFunctionFloat.BindDynamic(this, &ADoorBase::UpdateTimelineComp);
	if (DoorTimelineFloatCurve)
	{
		DoorTimelineComp->AddInterpFloat(DoorTimelineFloatCurve, UpdateFunctionFloat);
	}
	DoorProxVolume->OnComponentBeginOverlap.AddDynamic(this, &ADoorBase::OnOverlapBegin);
	DoorProxVolume->OnComponentEndOverlap.AddDynamic(this, &ADoorBase::OnOverlapEnd);

}

// Called every frame
void ADoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorBase::Interact_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("Door Interacted! Is Open: %s"), CurrentStatus);
	if (bIsCanInteract && CurrentStatus == EDoorStatus::Open)
	{
		DoorTimelineComp->Reverse();
		CurrentStatus = EDoorStatus::Close;
	}
	else if (bIsCanInteract && CurrentStatus == EDoorStatus::Close)
	{
		DoorTimelineComp->Play();
		CurrentStatus = EDoorStatus::Open;
	}
}

bool ADoorBase::IsOpen() const
{
	return CurrentStatus == EDoorStatus::Open;
}

void ADoorBase::UpdateTimelineComp(float Output)
{
	FRotator DoorNewRotation = FRotator(0.0f,Output,0.0f);
	DoorMesh -> SetRelativeRotation(DoorNewRotation);
}

void ADoorBase::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	bIsCanInteract = true;
}
 
void ADoorBase::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	bIsCanInteract = false;
}



