// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieAIComponent.h"

#include "Net/UnrealNetwork.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Character/Zombie/State/FZombieAlertState.h"
#include "Outbreak/Character/Zombie/State/FZombieAttackState.h"
#include "Outbreak/Character/Zombie/State/FZombieChaseState.h"
#include "Outbreak/Character/Zombie/State/FZombieDieState.h"
#include "Outbreak/Character/Zombie/State/FZombieIdleState.h"
#include "Outbreak/Character/Zombie/State/FZombieWanderState.h"
#include "Outbreak/Util/Define.h"
#include "Utilities/DebugHelper.h"

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

	OwnerZombie = Cast<ACharacterZombie>(InPawn);
	if (!OwnerZombie)
	{
		PRINT_WITH_CURRENT_CONTEXT("Pawn is not ACharacterZombie");
		return;
	}
	SetupAIPerception();
	SetupStateMachine();
	
	OwnerZombie->OnDeathDelegate.AddDynamic(this, &AZombieAIComponent::HandleOwnerDeath);
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
	if (!HasAuthority()) return;
	
	if (Stimulus.WasSuccessfullySensed())
	{
		const TObjectPtr<ACharacterPlayer> TargetPlayer = Cast<ACharacterPlayer>(Actor);
		CurrentTargetPlayer = TargetPlayer;

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
	if (!OwnerZombie || !SightConfig) return;
	
	const auto* Data = OwnerZombie->GetZombieData();
	SightConfig->SightRadius = Data->SightRadius;
	SightConfig->LoseSightRadius = Data->LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = Data->PeripheralVisionAngleDegrees;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 1000.0f;
	SightConfig->SetMaxAge(10.0f);

	AIPerception->ConfigureSense(*SightConfig);
}

void AZombieAIComponent::SetupStateMachine()
{
	if (!OwnerZombie) return;
	
	StateMachine = MakeShared<FZombieStateMachine>();
	StateMachine->AddState(EZombieStateType::Idle, MakeShared<FZombieIdleState>(StateMachine, OwnerZombie));
	StateMachine->AddState(EZombieStateType::Wander, MakeShared<FZombieWanderState>(StateMachine, OwnerZombie));
	StateMachine->AddState(EZombieStateType::Alert, MakeShared<FZombieAlertState>(StateMachine, OwnerZombie));
	StateMachine->AddState(EZombieStateType::Chase, MakeShared<FZombieChaseState>(StateMachine, OwnerZombie));
	StateMachine->AddState(EZombieStateType::Attack, MakeShared<FZombieAttackState>(StateMachine, OwnerZombie));
	StateMachine->AddState(EZombieStateType::Die, MakeShared<FZombieDieState>(StateMachine, OwnerZombie));
	
	StateMachine->ChangeState(EZombieStateType::Idle);
}

void AZombieAIComponent::HandleOwnerDeath(AActor* DeadActor)
{
	if (StateMachine.IsValid())
	{
		StateMachine->ChangeState(EZombieStateType::Die);
	}
}
