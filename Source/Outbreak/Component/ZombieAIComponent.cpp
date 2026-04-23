// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieAIComponent.h"

#include "MassEntity/ZombieEntityBase.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Character/Zombie/State/FZombieAlertState.h"
#include "Outbreak/Character/Zombie/State/FZombieAttackState.h"
#include "Outbreak/Character/Zombie/State/FZombieChaseState.h"
#include "Outbreak/Character/Zombie/State/FZombieDieState.h"
#include "Outbreak/Character/Zombie/State/FZombieIdleState.h"
#include "Outbreak/Character/Zombie/State/FZombieWanderState.h"
#include "Outbreak/Util/Define.h"

AZombieAIComponent::AZombieAIComponent()
{
	PrimaryActorTick.bCanEverTick = true;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AZombieAIComponent::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if WITH_EDITOR
	SetFolderPath(TEXT("AIComponents"));
#endif
}

void AZombieAIComponent::BeginPlay()
{
	Super::BeginPlay();

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AZombieAIComponent::OnTargetPerceptionUpdated);
}

void AZombieAIComponent::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (InPawn != nullptr && !IsValid(InPawn))
	{
		return;
	}
	
	if (ACharacterZombie* OwnerZombie = Cast<ACharacterZombie>(InPawn))
	{
		OwnerZombie->OnCharacterDeathDelegate.AddDynamic(this, &AZombieAIComponent::HandleOwnerDeath);
	}
	else if (AZombieEntityBase* OwnerZombieEntity = Cast<AZombieEntityBase>(InPawn))
	{
		OwnerZombieEntity->OnCharacterDeathDelegate.AddDynamic(this, &AZombieAIComponent::HandleOwnerDeath);
	}
	
	SetupAIPerception();
	SetupStateMachine();
}

void AZombieAIComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority()) return;
	
	if (StateMachine.IsValid())
	{
		StateMachine->Execute(DeltaTime);
	}
}

void AZombieAIComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentTargetPlayer);
}

void AZombieAIComponent::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!HasAuthority() || CurrentTargetPlayer)
	{
		return;
	}
	
	if (Stimulus.WasSuccessfullySensed())
	{
		const TObjectPtr<ACharacterPlayer> TargetPlayer = Cast<ACharacterPlayer>(Actor);
		CurrentTargetPlayer = TargetPlayer;
		CurrentTargetPlayer->OnCharacterDeathDelegate.AddUniqueDynamic(this, &AZombieAIComponent::HandleTargetDeath);

		EZombieStateType CurrentState = EZombieStateType::None;
		if (StateMachine.IsValid())
		{
			CurrentState = StateMachine->GetCurrentState();
		}
		
		if (CurrentState == EZombieStateType::Idle || CurrentState == EZombieStateType::Wander)
		{
			StateMachine->ChangeState(EZombieStateType::Alert);
		}
	}
	else
	{
		StateMachine->ChangeState(EZombieStateType::Wander);
		CurrentTargetPlayer = nullptr;
	}
}

void AZombieAIComponent::SetupAIPerception()
{
	if (!SightConfig || !GetPawn())
	{
		return;
	}
	
	FZombieData* Data = nullptr;
	if (ACharacterZombie* OwnerZombie = GetPawn<ACharacterZombie>())
	{
		Data = OwnerZombie->GetZombieData();
	}
	else if (AZombieEntityBase* OwnerZombieEntity = GetPawn<AZombieEntityBase>())
	{
		Data = OwnerZombieEntity->GetZombieData();
	}
	
	SightConfig->SightRadius = Data->SightRadius;
	SightConfig->LoseSightRadius = Data->LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = Data->PeripheralVisionAngleDegrees;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 1000.0f;
	SightConfig->SetMaxAge(10.0f);

	AIPerception->ConfigureSense(*SightConfig);
}

void AZombieAIComponent::SetupStateMachine()
{
	if (ACharacterZombie* OwnerZombie = Cast<ACharacterZombie>(GetPawn()))
	{
		StateMachine = MakeShared<FZombieStateMachine>();
		StateMachine->AddState(EZombieStateType::Idle, MakeShared<FZombieIdleState>(StateMachine, OwnerZombie));
		StateMachine->AddState(EZombieStateType::Wander, MakeShared<FZombieWanderState>(StateMachine, OwnerZombie));
		StateMachine->AddState(EZombieStateType::Alert, MakeShared<FZombieAlertState>(StateMachine, OwnerZombie));
		StateMachine->AddState(EZombieStateType::Chase, MakeShared<FZombieChaseState>(StateMachine, OwnerZombie));
		StateMachine->AddState(EZombieStateType::Attack, MakeShared<FZombieAttackState>(StateMachine, OwnerZombie));
		StateMachine->AddState(EZombieStateType::Die, MakeShared<FZombieDieState>(StateMachine, OwnerZombie));
		
		StateMachine->ChangeState(EZombieStateType::Idle);
	}
	else if (AZombieEntityBase* OwnerZombieEntity = Cast<AZombieEntityBase>(GetPawn()))
	{
		StateMachine = MakeShared<FZombieStateMachine>();
		StateMachine->AddState(EZombieStateType::Idle, MakeShared<FZombieIdleState>(StateMachine, OwnerZombieEntity));
		StateMachine->AddState(EZombieStateType::Wander, MakeShared<FZombieWanderState>(StateMachine, OwnerZombieEntity));
		StateMachine->AddState(EZombieStateType::Alert, MakeShared<FZombieAlertState>(StateMachine, OwnerZombieEntity));
		StateMachine->AddState(EZombieStateType::Chase, MakeShared<FZombieChaseState>(StateMachine, OwnerZombieEntity));
		StateMachine->AddState(EZombieStateType::Attack, MakeShared<FZombieAttackState>(StateMachine, OwnerZombieEntity));
		StateMachine->AddState(EZombieStateType::Die, MakeShared<FZombieDieState>(StateMachine, OwnerZombieEntity));
		
		StateMachine->ChangeState(EZombieStateType::Idle);
	}
}

void AZombieAIComponent::HandleOwnerDeath(AActor* DeadActor)
{
	if (!HasAuthority()) return;

	if (StateMachine.IsValid())
	{
		StateMachine->ChangeState(EZombieStateType::Die);
	}
}

void AZombieAIComponent::HandleTargetDeath(AActor* DeadActor)
{
	if (!HasAuthority()) return;

	if (CurrentTargetPlayer == DeadActor)
	{
		CurrentTargetPlayer->OnCharacterDeathDelegate.RemoveDynamic(this, &AZombieAIComponent::HandleTargetDeath);
		CurrentTargetPlayer = nullptr;
		ACharacterPlayer* DeadPlayer = Cast<ACharacterPlayer>(DeadActor);
		if (IsValid(DeadPlayer))
		{
			DeadPlayer->OnCharacterDeathDelegate.RemoveDynamic(this, &AZombieAIComponent::HandleTargetDeath);
		}
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AZombieAIComponent::FindNewTarget, 0.1f, false);	} 
}

void AZombieAIComponent::FindNewTarget()
{
	TArray<AActor*> PerceivedActors;
	AIPerception->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

	ACharacterPlayer* BestTarget = nullptr;
	float MinDistance = MAX_FLT;

	for (AActor* Actor : PerceivedActors)
	{
		ACharacterPlayer* Player = Cast<ACharacterPlayer>(Actor);
		if (Player && !Player->IsDead())
		{
			float Distance = FVector::Dist(GetPawn()->GetActorLocation(), Player->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				BestTarget = Player;
			}
		}
	}

	if (BestTarget)
	{
		CurrentTargetPlayer = BestTarget;
		CurrentTargetPlayer->OnCharacterDeathDelegate.AddUniqueDynamic(this, &AZombieAIComponent::HandleTargetDeath);
		StateMachine->ChangeState(EZombieStateType::Chase);
	}
	else
	{
		CurrentTargetPlayer = nullptr;
		StateMachine->ChangeState(EZombieStateType::Wander);
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}
