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
	Close,
	None
};

UCLASS()
class OUTBREAK_API ADoorBase : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:
	ADoorBase();

	// 멤버 함수
public:
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	bool IsOpen() const;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DoorTimelineFloatCurve;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void UpdateTimeline(float Output);

	
	// 멤버 변수
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> DoorTimelineFloatCurve;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> DoorFrame;
    
	TObjectPtr<UTimelineComponent> DoorTimelineComp;
    
	bool bIsCanInteract = true;
	bool bIsDestroyable = false;
	int HP = 100;
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
	FRotator InitialRotation;
	FRotator TargetRotation;
};