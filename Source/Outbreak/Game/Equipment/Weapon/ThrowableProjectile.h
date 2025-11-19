#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Outbreak/Data/GameData.h"
#include "ThrowableProjectile.generated.h"

UCLASS()
class OUTBREAK_API AThrowableProjectile : public AActor
{
	GENERATED_BODY()
	//-----Function-----//
public:
	AThrowableProjectile();

	void InitializeProjectile(const FVector& ShootDirection, const struct FThrowableData& Data);
	void Explode();

protected:
	virtual void BeginPlay() override;


	//-----Variables-----//
protected:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	class USkeletalMeshComponent* MeshComponent;

	UPROPERTY()
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleInstanceOnly, Category = "Data")
	FThrowableData ThrowableData;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ULegacyCameraShake> CameraShakeClass;

	FTimerHandle ExplosionTimerHandle;
};
