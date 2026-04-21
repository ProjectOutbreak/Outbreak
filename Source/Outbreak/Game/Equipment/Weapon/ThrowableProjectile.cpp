#include "ThrowableProjectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Util/CameraShake.h"


AThrowableProjectile::AThrowableProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->InitSphereRadius(10.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
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

	bReplicates = true;
	SetReplicateMovement(true);
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
	Multicast_Explode();
	
	DrawDebugSphere(GetWorld(), GetActorLocation(), ThrowableData.ExplosionRadius, 12, FColor::Red, false, 3.0f);
	SetLifeSpan(0.1f);
}

void AThrowableProjectile::Multicast_Explode_Implementation()
{
	if (ThrowableData.ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ThrowableData.ExplosionSound, GetActorLocation());
	}
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
}

void AThrowableProjectile::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThrowableProjectile, ThrowableData);
}



