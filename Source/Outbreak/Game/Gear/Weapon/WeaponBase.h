// WeaponBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Util/Define.h"
#include "WeaponBase.generated.h"

class USkeletalMeshComponent;
class AAmmoBase;

UCLASS(Abstract)
class OUTBREAK_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void StartFire();
	virtual void StopFire();
	virtual void Reload();
	virtual void InitializeWeaponData(FWeaponData* InData);
	virtual void NotifyAmmoUpdate();
	virtual void BeginPlay() override;
	bool IsReloading() const { return bIsReloading; }


protected:
	UPROPERTY(VisibleAnywhere, Category="Weapon")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(Replicated)
	bool bIsReloading = false;
};
