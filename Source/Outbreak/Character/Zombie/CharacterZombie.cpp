// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterZombie.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "Outbreak/Component/ZombieAIComponent.h"
#include "Outbreak/Game/Framework/InGameMode.h"
#include "Outbreak/Game/Framework/InGameState.h"
#include "Outbreak/Game/Framework/DefaultPlayerState.h"
#include "Outbreak/Manager/CharacterSpawnManager.h"
#include "Outbreak/Util/EnumHelper.h"
#include "Outbreak/Util/MeshLoadHelper.h"

ACharacterZombie::ACharacterZombie()
{
	CharacterType = ECharacterType::Zombie;
	AIControllerClass = AZombieAIComponent::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	RightHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandCollision"));
	RightHandCollision->SetupAttachment(GetMesh(), TEXT("hand_r_socket"));
    
	RightHandCollision->SetHiddenInGame(false);
	RightHandCollision->SetSphereRadius(40.0f);
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollision->SetCollisionObjectType(ECC_WorldDynamic);
	RightHandCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &ACharacterZombie::OnAttackOverlapBegin);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMesh(TEXT("/Game/Art/Characters/Zombies/Meshes/SKM_Zombie_Normal_001.SKM_Zombie_Normal_001"));
	if (DefaultMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(DefaultMesh.Object);
	}
	
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Blueprints/ABP_Zombie.ABP_Zombie_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> DeadSoundCueFinder(TEXT("/Game/Audio/SFX/Cues/Zombies/SC_ZombieDead.SC_ZombieDead"));
	if (DeadSoundCueFinder.Succeeded())
	{
		DeadSoundCue = DeadSoundCueFinder.Object;
	}
}

void ACharacterZombie::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ZombieData);
	DOREPLIFETIME(ThisClass, bIsAttacking);
	DOREPLIFETIME(ThisClass, bIsAlert);
}

void ACharacterZombie::InitCharacterData()
{
	Super::InitCharacterData();
	
	if (HasAuthority())
	{
		const AInGameMode* GameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (!GameMode) return;
		
		ACharacterSpawnManager* SpawnManager = GameMode->GetSpawnManager();
		if (!SpawnManager) return;
		
		const FZombieData* Data = SpawnManager->GetZombieData(ZombieSubType);
		if (!Data) return;
		
		ZombieData = *Data;
		ApplyZombieData();
	}
	
	CurrentExtraHealth = 0;
}

void ACharacterZombie::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if WITH_EDITOR
	SetFolderPath(TEXT("Zombies"));
#endif
}

void ACharacterZombie::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && !AlreadyHitActors.Contains(OtherActor))
	{
		if (ACharacterPlayer* HitPlayer = Cast<ACharacterPlayer>(OtherActor))
		{
			if (HitPlayer->IsDead()) return;

			AlreadyHitActors.Add(OtherActor);

			const FZombieData* Data = GetZombieData();
			UGameplayStatics::ApplyDamage(
				HitPlayer,
				Data ? Data->AttackDamage : 10.0f,
				GetController(),
				this,
				UDamageType::StaticClass()
			);
		}
	}
}

void ACharacterZombie::EnableAttackCollision()
{
	AlreadyHitActors.Empty();
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ACharacterZombie::DisableAttackCollision()
{
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACharacterZombie::SetupCollision()
{
	Super::SetupCollision();

	GetCapsuleComponent()->InitCapsuleSize(DefaultCapsuleRadius, DefaultCapsuleHalfHeight);
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	GetCapsuleComponent()->SetNotifyRigidBodyCollision(false);
	GetCapsuleComponent()->BodyInstance.bLockXRotation = true;
	GetCapsuleComponent()->BodyInstance.bLockYRotation = true;
	GetCapsuleComponent()->BodyInstance.bLockZRotation = false;
    
	GetCapsuleComponent()->BodyInstance.LinearDamping = 1.0f;
	GetCapsuleComponent()->BodyInstance.AngularDamping = 10.0f;
}

void ACharacterZombie::SetupMovement()
{
	Super::SetupMovement();

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	MovementComp->bOrientRotationToMovement = false;
	MovementComp->bUseControllerDesiredRotation = true;
	MovementComp->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	MovementComp->MaxAcceleration = 1024.0f;
	MovementComp->AvoidanceConsiderationRadius = 500.0f;
	MovementComp->SetAvoidanceEnabled(true);
	MovementComp->SetRVOAvoidanceWeight(0.3f);
	MovementComp->SetAvoidanceGroup(static_cast<int32>(EAvoidanceGroupType::Zombie));
	MovementComp->SetGroupsToAvoid(static_cast<int32>(EAvoidanceGroupType::Zombie));
	MovementComp->SetGroupsToIgnore(static_cast<int32>(EAvoidanceGroupType::Player));

	MovementComp->JumpZVelocity = 0.0f;
}

void ACharacterZombie::OnRep_Die()
{
	Super::OnRep_Die();
	
	if (DeadSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeadSoundCue, GetActorLocation());
	}
	
	if (HasAuthority())
	{
		OnDeathDelegate.Broadcast(this);
		
		if (LastDamagePlayer)
		{
			if (ADefaultPlayerState* PS = LastDamagePlayer->GetPlayerState<ADefaultPlayerState>())
			{
				PS->AddZombieKill();
			}
		}
		if (AInGameState* GS = GetWorld()->GetGameState<AInGameState>())
		{
			GS->AddTotalZombieKill();
		}
	}
}

float ACharacterZombie::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	LastDamagePlayer = EventInstigator; // Save Last Instigator
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACharacterZombie::SetMesh(const ECharacterBodyType MeshType)
{
	const FString MeshTypeString = EnumHelper::EnumToString(MeshType);
	
	// TODO : Zombie Mesh Data Manage
	int NormalMesh = 20;
	int MuscleMesh = 6;
	int FatMesh = 6;

	int MeshCount = 0;
	switch (MeshType)
	{
		case ECharacterBodyType::Normal:
			MeshCount = NormalMesh;
			break;
		case ECharacterBodyType::Muscle:
			MeshCount = MuscleMesh;
			break;
		case ECharacterBodyType::Fat:
			MeshCount = FatMesh;
			break;
	}

	if (const TObjectPtr<USkeletalMesh> ZombieMesh = MeshLoadHelper::GetRandomZombieMesh(MeshTypeString, MeshCount))
	{
		GetMesh()->SetSkeletalMesh(ZombieMesh);
	}
}

void ACharacterZombie::OnRep_ZombieData()
{
	ApplyZombieData();
}

void ACharacterZombie::SetIsAttacking(const bool bInIsAttacking)
{
	if (!HasAuthority()) return;
	
	bIsAttacking = bInIsAttacking;
}

void ACharacterZombie::SetIsAlert(const bool bInIsAlert)
{
	if (!HasAuthority()) return;
	
	bIsAlert = bInIsAlert;
}

void ACharacterZombie::ApplyZombieData()
{
	CurrentHealth = ZombieData.MaxHealth;
}
