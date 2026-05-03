// Fill out your copyright notice in the Description page of Project Settings.

#include "MassEntity/ZombieEntityBase.h"

#include "Component/FootStepComponent.h"
#include "Component/ZombieAIComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "Game/Framework/DefaultPlayerState.h"
#include "Game/Framework/InGameMode.h"
#include "Game/Framework/InGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/CharacterSpawnManager.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/OutbreakStatics.h"

AZombieEntityBase::AZombieEntityBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	AIControllerClass = AZombieAIComponent::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	float CapsuleHalfHeight = 0.f;
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionProfileName(TEXT("Pawn"));
		CapsuleComp->InitCapsuleSize(42.f, 96.0f);
		CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();
		
		CapsuleComp->InitCapsuleSize(DefaultCapsuleRadius, DefaultCapsuleHalfHeight);
		CapsuleComp->SetNotifyRigidBodyCollision(false);
		CapsuleComp->BodyInstance.bLockXRotation = true;
		CapsuleComp->BodyInstance.bLockYRotation = true;
		CapsuleComp->BodyInstance.bLockZRotation = false;
    
		CapsuleComp->BodyInstance.LinearDamping = 1.0f;
		CapsuleComp->BodyInstance.AngularDamping = 10.0f;
	}

	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		SkeletalMeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		SkeletalMeshComp->SetCollisionProfileName(TEXT("CharacterMesh"));
		SkeletalMeshComp->SetHiddenInGame(false);
		SkeletalMeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -CapsuleHalfHeight),FRotator(0.0f, -90.0f, 0.0f));
	}
	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->JumpZVelocity = 0.f;
		MovementComp->AirControl = 0.35f;
		MovementComp->MaxWalkSpeed = 500.f;
		MovementComp->MinAnalogWalkSpeed = 20.f;
		MovementComp->BrakingDecelerationWalking = 2000.f;
		MovementComp->GravityScale = 1.5f;
		MovementComp->GroundFriction = 8.0f;

		MovementComp->bCanWalkOffLedges = true;
		MovementComp->bUseFlatBaseForFloorChecks = true;
		MovementComp->bMaintainHorizontalGroundVelocity = true;
		MovementComp->bImpartBaseVelocityX = false;
		MovementComp->bImpartBaseVelocityY = false;
		MovementComp->bImpartBaseVelocityZ = false;
		
		MovementComp->bOrientRotationToMovement = false;
		MovementComp->bUseControllerDesiredRotation = true;
		MovementComp->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
		MovementComp->MaxAcceleration = 1024.0f;
	}
	
	FootStepComponent = CreateDefaultSubobject<UFootStepComponent>(TEXT("FootStepComponent"));
	
	RightHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandCollision"));
	RightHandCollision->SetupAttachment(GetMesh(), TEXT("hand_r_socket"));
	RightHandCollision->SetHiddenInGame(true);
	RightHandCollision->SetSphereRadius(40.0f);
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollision->SetCollisionObjectType(ECC_WorldDynamic);
	RightHandCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAttackOverlapBegin);
	
	static ConstructorHelpers::FObjectFinder<USoundCue> DeadSoundCueFinder(TEXT("/Game/Audio/SFX/Cues/Zombies/SC_ZombieDead.SC_ZombieDead"));
	if (DeadSoundCueFinder.Succeeded())
	{
		DeadSoundCue = DeadSoundCueFinder.Object;
	}

}

void AZombieEntityBase::SetEnableAttackCollision(const bool bEnable)
{
	AlreadyHitActors.Empty();
	RightHandCollision->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AZombieEntityBase::SetIsAttacking(const bool bInIsAttacking)
{
	if (HasAuthority())
	{
		bIsAttacking = bInIsAttacking;
	}
}

void AZombieEntityBase::OnRagdoll() const
{
	FVector LastVelocity = FVector::ZeroVector;
	
	if (UCharacterMovementComponent* CharMoveComp = GetCharacterMovement())
	{
		LastVelocity = CharMoveComp->Velocity;
		CharMoveComp->DisableMovement();
	}
	
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComp->SetSimulatePhysics(true);
		MeshComp->AddImpulse(LastVelocity, NAME_None, true);
	}
}

void AZombieEntityBase::ApplyDamage(int32 InDamageAmount)
{
	if (!HasAuthority())
	{
		return;
	}
	
	CurrentHealth = FMath::Max(0, CurrentHealth - InDamageAmount);
	
	if (IsDead())
	{
		Die();
	}
	else
	{
		// TODO : Implement hit reaction logic (maybe animation)
	}
}

void AZombieEntityBase::Die()
{
	if (!HasAuthority())
	{
		return;
	}
	
	bIsDead = true;
	OnRep_IsDead();
	OnCharacterDeathDelegate.Broadcast(this);
}

void AZombieEntityBase::OnRep_IsDead()
{
	if (DeadSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeadSoundCue, GetActorLocation());
	}
	
	if (HasAuthority())
	{
		if (LastDamagedPlayer.IsValid())
		{
			if (ADefaultPlayerState* PS = LastDamagedPlayer.Get()->GetPlayerState<ADefaultPlayerState>())
			{
				PS->AddZombieKill();
			}
		}
		if (AInGameState* GS = GetWorld()->GetGameState<AInGameState>())
		{
			GS->AddTotalZombieKill();
		}
	}
	
	OnRagdoll();
	SetLifeSpan(10.0f);
}

void AZombieEntityBase::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && !AlreadyHitActors.Contains(OtherActor))
	{
		if (ACharacterPlayer* HitPlayer = Cast<ACharacterPlayer>(OtherActor))
		{
			if (HitPlayer->IsDead())
			{
				return;
			}

			AlreadyHitActors.Add(OtherActor);

			const float Damage = ZombieData.AttackDamage;
			UGameplayStatics::ApplyDamage(HitPlayer, Damage, GetController(), this, UDamageType::StaticClass());
		}
		
		OnAttackOtherCharacter.Broadcast(OtherActor);
	}
}

void AZombieEntityBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		const FZombieData* Data = UOutbreakStatics::GetZombieData(this, EZombieSubType::Runner);
		if (!Data)
		{
			UE_LOG(LogTemp, Error, TEXT("Fail to get FZombieData"));
			return;
		}
		ZombieData = *Data;
	}
	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->AvoidanceConsiderationRadius = 500.0f;
		MovementComp->SetAvoidanceEnabled(true);
		MovementComp->SetRVOAvoidanceWeight(0.3f);
		MovementComp->SetAvoidanceGroup(static_cast<int32>(EAvoidanceGroupType::Zombie));
		MovementComp->SetGroupsToAvoid(static_cast<int32>(EAvoidanceGroupType::Zombie));
		MovementComp->SetGroupsToIgnore(static_cast<int32>(EAvoidanceGroupType::Player));
	}
}

void AZombieEntityBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
#if WITH_EDITOR
	SetFolderPath(TEXT("Zombies"));
#endif
}

void AZombieEntityBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, ZombieData);
	DOREPLIFETIME(ThisClass, bIsAttacking);
}

float AZombieEntityBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDead())
	{
		return 0.0f;
	}
	
	int32 DamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.IsOfType((FPointDamageEvent::ClassID)))
	{
		const auto PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		const FHitResult& HitResult = PointDamageEvent->HitInfo;
		const UPhysicalMaterial* PhysMat = HitResult.PhysMaterial.Get();
		if (PhysMat)
		{
			DamageAmount *= UOutbreakStatics::GetDamageMultiplier(PhysMat->SurfaceType);
		}
	}
	
	ApplyDamage(DamageAmount);
	
	LastDamagedPlayer = EventInstigator;
		
	return DamageAmount;
}
