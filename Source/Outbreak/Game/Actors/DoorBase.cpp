// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorBase.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"

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
	InitialRotation = DoorMesh->GetRelativeRotation();
	if (DoorMesh)
	{
		InitialRotation = DoorMesh->GetRelativeRotation();
	}
    
	// 타임라인에 업데이트 함수를 바인딩합니다.
	if (DoorTimelineComp)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateTimeline"));
		DoorTimelineComp->AddInterpFloat(DoorTimelineFloatCurve, TimelineCallback);
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
	UWorld* World = GetWorld();
	if (World)
	{
		FVector DoorLocation = GetActorLocation();
		FVector PlayerLocation = InstigatorPawn->GetActorLocation();
        
		// 1. 문의 '앞' 방향 (Forward Vector)을 빨간색 선으로 그립니다.
		FVector DoorForwardVector = GetActorForwardVector();
		DrawDebugLine(World, DoorLocation, DoorLocation + DoorForwardVector * 150.0f, FColor::Red, false, 5.0f, 0, 5.0f);

		// 2. 문에서 플레이어로 향하는 방향을 파란색 선으로 그립니다.
		FVector DirectionToPlayer = (PlayerLocation - DoorLocation).GetSafeNormal();
		DrawDebugLine(World, DoorLocation, DoorLocation + DirectionToPlayer * 150.0f, FColor::Blue, false, 5.0f, 0, 5.0f);
        
		// 3. 내적(Dot Product) 결과를 화면에 출력합니다.
		float DotProduct = FVector::DotProduct(DoorForwardVector, DirectionToPlayer);
		FString DebugText = FString::Printf(TEXT("Dot Product: %.2f"), DotProduct);
		DrawDebugString(World, DoorLocation + FVector(0, 0, 100.0f), DebugText, nullptr, FColor::White, 5.0f, true);
	}
		
	if (!DoorMesh || !DoorTimelineComp || !InstigatorPawn)
	{
		return;
	}

	// 2. 현재 문 상태에 따라 열거나 닫는 동작을 결정합니다.
	if (CurrentStatus == EDoorStatus::Close)
	{
		// [문 열기 로직]
        
		// 목표 회전 값을 계산하기 전, 초기 회전 값으로 초기화합니다.
		TargetRotation = InitialRotation;

		// 플레이어 위치를 기준으로 문이 밀리도록 목표 회전 값을 계산합니다.
		FVector DoorLocation = GetActorLocation();
		FVector PlayerLocation = InstigatorPawn->GetActorLocation();
		FVector DirectionToPlayer = (PlayerLocation - DoorLocation).GetSafeNormal();
        
		// 문짝 메시(DoorMesh)의 정면 방향을 기준으로 계산합니다.
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
        
		// 상태를 'Open'으로 변경하고 타임라인을 처음부터 재생합니다.
		CurrentStatus = EDoorStatus::Open;
		DoorTimelineComp->PlayFromStart();
	}
	else if (CurrentStatus == EDoorStatus::Open)
	{
		// [문 닫기 로직]

		// 상태를 'Close'로 변경하고 타임라인을 끝에서부터 역재생합니다.
		CurrentStatus = EDoorStatus::Close;
		DoorTimelineComp->ReverseFromEnd();
	}

	// 최종적으로 계산된 목표 회전 값으로 문을 회전시킵니다.
	DoorMesh->SetRelativeRotation(TargetRotation);
}

bool ADoorBase::IsOpen() const
{
	return CurrentStatus == EDoorStatus::Open;
}

void ADoorBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("OnOverlapBegin"));
	ACharacterPlayer* PlayerCharacter = Cast<ACharacterPlayer>(OtherActor);
	if (PlayerCharacter)
	{
		// Add this line to log the character's name
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter Detected: %s"), *PlayerCharacter->GetName());

		PlayerCharacter->SetCurrentInteractable(this);
	}
	bIsCanInteract = true;
}
 
void ADoorBase::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Display, TEXT("OnOverlapEnd"));
	ACharacterPlayer* PlayerCharacter = Cast<ACharacterPlayer>(OtherActor);
	if (PlayerCharacter)
	{
		// 플레이어에게 상호작용 대상을 초기화하라고 요청합니다.
		PlayerCharacter->ClearCurrentInteractable(this);
	}
	bIsCanInteract = false;
}

void ADoorBase::UpdateTimeline(float Output)
{
	// 커브의 Output 값을 사용하여 InitialRotation과 TargetRotation 사이를 보간(Lerp)합니다.
	FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Output);
	DoorMesh->SetRelativeRotation(NewRotation);
}

