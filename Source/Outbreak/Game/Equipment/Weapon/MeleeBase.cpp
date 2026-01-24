#include "MeleeBase.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


AMeleeBase::AMeleeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMeleeBase::BeginPlay()
{
	Super::BeginPlay();
	CachedOwnerCharacter = Cast<ACharacter>(GetOwner());
}

void AMeleeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsTracing)
	{
		PerformHitDetection();
	}
}

void AMeleeBase::OnEquip()
{
	CachedOwnerCharacter = Cast<ACharacter>(GetOwner());
}

void AMeleeBase::OnEndUse()
{
	
}


bool AMeleeBase::CanUse() const
{
	return !bIsAttacking;
}

bool AMeleeBase::IsActive() const
{
	return bIsAttacking;
}

void AMeleeBase::OnUse()
{
	if (!CanUse()) return;
	bIsAttacking = true;
	
	if (HasAuthority())
	{
		Multicast_AttackAnim(Cast<ACharacter>(GetOwner()), MeleeData.AttackMontage);
	}
}

void AMeleeBase::Multicast_AttackAnim_Implementation(ACharacter* InOwner, UAnimMontage* MontageToPlay)
{
	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
	}
	if (InOwner && MontageToPlay)
	{
		UAnimInstance* AnimInstance = InOwner->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(MontageToPlay);
		}
	}
	else
	{
		ResetAttack();
	}
}


void AMeleeBase::ResetAttack()
{
	bIsAttacking = false;
}

void AMeleeBase::OnHitDetectionBegin()
{
	bIsTracing = true;
	HitActors.Empty();
}

void AMeleeBase::OnHitDetectionEnd()
{
	bIsTracing = false;
}

void AMeleeBase::PerformHitDetection()
{
	if (!CachedOwnerCharacter) return;
	
	AController* OwnerController = CachedOwnerCharacter->GetController();
	if (!OwnerController) return;

	const FVector Start = EquipmentMesh->GetSocketLocation(HitStartSocketName);
	FVector End = EquipmentMesh->GetSocketLocation(HitEndSocketName);

	FVector AttackDirection = (End - Start).GetSafeNormal();

	if (AttackDirection.IsNearlyZero()) AttackDirection = CachedOwnerCharacter->GetActorForwardVector();

	const FVector EndDirection = Start + (AttackDirection * MeleeData.AttackRange);

	const float Radius = 25.0f;

	TArray<FHitResult> OutHits;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(CachedOwnerCharacter);

	UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		Start,
		EndDirection,
		Radius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true
		);

	for (const FHitResult& Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		
		if (HitActor && !HitActors.Contains(HitActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Melee Hit Success: %s"), *HitActor->GetName());

			HitActors.Add(HitActor);

			UGameplayStatics::ApplyPointDamage(
				HitActor,
				MeleeData.Damage,
				(End - Start).GetSafeNormal(),
				Hit,
				OwnerController,
				this,
				nullptr
				);
		}
	}
}