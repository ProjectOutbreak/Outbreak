#include "ThrowableProjectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Util/CameraShake.h"


AThrowableProjectile::AThrowableProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->InitSphereRadius(10.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	CollisionComponent->SetSimulatePhysics(true);
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
	ProjectileMovementComponent->InitialSpeed = 1000.f;
	ProjectileMovementComponent->MaxSpeed = 1000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	
	CameraShakeClass = UCameraShake::StaticClass();
}

void AThrowableProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AThrowableProjectile::InitializeProjectile(const FVector& ShootDirection, const FThrowableData& Data)
{
	ThrowableData = Data;
	
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->InitialSpeed = ThrowableData.ThrowForce;
		ProjectileMovementComponent->MaxSpeed = ThrowableData.ThrowForce;
		ProjectileMovementComponent->Velocity = ShootDirection * ThrowableData.ThrowForce;
	}
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AThrowableProjectile::Explode, ThrowableData.FuseTime, false);
}

void AThrowableProjectile::Explode()
{
	UGameplayStatics::ApplyRadialDamage(
		this,
		ThrowableData.Damage,
		GetActorLocation(),
		ThrowableData.ExplosionRadius,
		UDamageType::StaticClass(),
		TArray<AActor*>(),
		this,
		GetInstigatorController(),
		true
		);
	if (ThrowableData.ExplosionVfx)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ThrowableData.ExplosionVfx, 
			GetActorLocation(),         
			GetActorRotation(),        
			ThrowableData.VFXScale,    
			true,                       
			true,                       
			ENCPoolMethod::None,
			true
			);
	}
	if (CameraShakeClass)
	{
		UGameplayStatics::PlayWorldCameraShake(
			GetWorld(),
			CameraShakeClass,
			GetActorLocation(),
			ThrowableData.ExplosionRadius,          
			ThrowableData.ExplosionRadius * 10.0f,  
			1.0f                                    
		);
	}
	DrawDebugSphere(GetWorld(), GetActorLocation(), ThrowableData.ExplosionRadius, 12, FColor::Red, false, 3.0f);
	Destroy();
}


