// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Outbreak/Game/Interface/InteractInterface.h"
#include "DoorBase.generated.h"

UENUM(BlueprintType)
enum class EDoorStatus : uint8
{
	Open,
	Close
};

UCLASS()
class OUTBREAK_API ADoorBase : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorBase();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	
	bool IsOpen() const;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DoorTimelineFloatCurve;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DoorMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DoorFrame;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	bool bIsCanInteract = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	bool bIsDestroyable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings", meta = (EditCondition = "bIsDestroyable"))
	int HP = 100;
	
	//UPROPERTY(Replicated, BlueprintReadOnly, Category = "Door State")
	EDoorStatus CurrentStatus = EDoorStatus::Close;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTimelineComponent* DoorTimelineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* DoorProxVolume;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	FOnTimelineFloat UpdateFunctionFloat;

	UFUNCTION()
	void UpdateTimeline(float Output);
	
	FRotator InitialRotation;

	FRotator TargetRotation;
};
