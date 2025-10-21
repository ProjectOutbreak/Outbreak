// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBase.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Util/Define.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/Public/Utilities/DebugHelper.h"
#include "Outbreak/UI/InGameHUD.h"
#include "Outbreak/Component/FootStepComponent.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	InitCharacterData();
	SetupCollision();
	SetupMovement();
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, CurrentHealth);
	DOREPLIFETIME(ACharacterBase, bIsDead);
	DOREPLIFETIME(ACharacterBase, bIsToxic);
}

float ACharacterBase::TakeDamage(const float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDead()) return 0.0f;
	
	int32 DamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.IsOfType((FPointDamageEvent::ClassID)))
	{
		const auto PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		const FHitResult& HitResult = PointDamageEvent->HitInfo;
		const UPhysicalMaterial* PhysMat = HitResult.PhysMaterial.Get();
		if (PhysMat)
		{
			DamageAmount *= GetDamageMultiplier(PhysMat->SurfaceType);
		}
	}
	
	ApplyDamage(DamageAmount);
		
	return DamageAmount;
}

void ACharacterBase::OnRep_CurrentHealth()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AInGameHUD* HUD = Cast<AInGameHUD>(PC->GetHUD()))
		{
			HUD->DisplayCurrentHealth(CurrentHealth);
		}
	}
}

void ACharacterBase::InitCharacterData()
{
	// Implement in derived classes
}

bool ACharacterBase::IsDead() const
{
	if (CurrentHealth <= 0)
	{
		return true;
	}
	return false;
}

void ACharacterBase::Die()
{
	if (!HasAuthority())
		return;

	bIsDead = true;

	OnRep_Die();
}

void ACharacterBase::OnRagdoll()
{
	const FVector LastVelocity = GetCharacterMovement()->Velocity;

	if (const AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* BrainComponent = AIController->GetBrainComponent())
		{
			BrainComponent->StopLogic("Death");
		}
	}

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->AddImpulse(LastVelocity, NAME_None, true);

	SetLifeSpan(10.0f);
}

void ACharacterBase::OnRep_Die()
{
	OnRagdoll();
}

float ACharacterBase::GetDamageMultiplier(const EPhysicalSurface SurfaceType)
{
	switch (SurfaceType)
	{
	case SurfaceType1: // Head
		return HeadDamageMultiplier;
	case SurfaceType2: // Body  
		return BodyDamageMultiplier;
	case SurfaceType3: // Limbs
		return LimbsDamageMultiplier;
	default:
		return 1.0f;
	}
}

void ACharacterBase::ApplyDamage(int32 DamageAmount)
{
	if (!HasAuthority()) return;

	const FString DebugMsg = FString::Printf(TEXT("Actor %s taking %d damage"), *GetName(), DamageAmount);
	PRINT_WITH_CURRENT_CONTEXT(*DebugMsg);
	
	const int32 DamageAbsorbedByExtraHealth = FMath::Min(DamageAmount, CurrentExtraHealth);
	CurrentExtraHealth -= DamageAbsorbedByExtraHealth;
	const int32 RemainingDamage = DamageAmount - DamageAbsorbedByExtraHealth;
	CurrentHealth = FMath::Max(0, CurrentHealth - RemainingDamage);

	if (IsDead())
	{
		Die();
	}
	else
	{
		// TODO : Implement hit reaction logic (maybe animation)
	}
}

void ACharacterBase::SetupCollision()
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if (!CapsuleComp) return;
	
	CapsuleComp->SetCollisionProfileName(TEXT("Pawn"));
	CapsuleComp->InitCapsuleSize(42.f, 96.0f);

	USkeletalMeshComponent* SkeletalMeshComp = GetMesh();
	if (!SkeletalMeshComp) return;
	
	SkeletalMeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	SkeletalMeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	SkeletalMeshComp->SetCollisionProfileName(TEXT("CharacterMesh"));
	SkeletalMeshComp->SetHiddenInGame(false);
}

void ACharacterBase::SetupMovement()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	FootStepComponent = NewObject<UFootStepComponent>(this, TEXT("FootStepComponent"));

	if (FootStepComponent)  FootStepComponent->RegisterComponent();
	if (!MovementComp) return;
	
	MovementComp->JumpZVelocity = 500.f;
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
}

void ACharacterBase::ApplyToxicDamage(float DamagePerSecond, float Duration)
{
	if (!HasAuthority())
		return;

	GetWorldTimerManager().ClearTimer(ToxicTickTimerHandle);
	GetWorldTimerManager().ClearTimer(ToxicDurationTimerHandle);

	ToxicDamagePerTick = DamagePerSecond;

	GetWorldTimerManager().SetTimer(ToxicTickTimerHandle, this, &ACharacterBase::ApplyToxicTick, 1.0f, true);
	GetWorldTimerManager().SetTimer(ToxicDurationTimerHandle, this, &ACharacterBase::ClearToxicEffect, Duration, false);
    
	if (!bIsToxic)
	{
		bIsToxic = true;
		OnRep_IsToxic();
	}
}

void ACharacterBase::OnRep_IsToxic()
{
	if (bIsToxic)
	{
		// TODO : Visual, Audio Effect
	}
	else
	{
		// TODO : Clear Visual, Audio Effect
	}
}

void ACharacterBase::ApplyToxicTick()
{
	ApplyDamage(ToxicDamagePerTick);
}

void ACharacterBase::ClearToxicEffect()
{
	GetWorldTimerManager().ClearTimer(ToxicTickTimerHandle);
	GetWorldTimerManager().ClearTimer(ToxicDurationTimerHandle);

	ToxicDamagePerTick = 0.f;
	
	if (bIsToxic)
	{
		bIsToxic = false;
		OnRep_IsToxic();
	}
}

void ACharacterBase::TriggerFootStepLeft()
{
	if (FootStepComponent) FootStepComponent->HandleFootStep(TEXT("ik_foot_l"));
}

void ACharacterBase::TriggerFootStepRight()
{
	if (FootStepComponent) FootStepComponent->HandleFootStep(TEXT("ik_foot_r"));
}