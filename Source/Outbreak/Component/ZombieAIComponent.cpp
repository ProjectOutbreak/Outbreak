// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieAIComponent.h"

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

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
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

void AZombieAIComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (StateMachine.IsValid())
	{
		StateMachine->Execute(DeltaTime);
	}
}

void AZombieAIComponent::InitializeZombieAI(ACharacterZombie* InZombie)
{
	OwnerZombie = InZombie;

	const auto* Data = OwnerZombie->GetZombieData();
	SightConfig->SightRadius = Data->SightRadius;
	SightConfig->LoseSightRadius = Data->LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = Data->PeripheralVisionAngleDegrees;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 1000.0f;
	SightConfig->SetMaxAge(10.0f);

	AIPerception->ConfigureSense(*SightConfig);
	
	StateMachine = MakeShared<FZombieStateMachine>();
	StateMachine->AddState(EZombieStateType::Idle, MakeShared<FZombieIdleState>(StateMachine, OwnerZombie));
	StateMachine->AddState(EZombieStateType::Wander, MakeShared<FZombieWanderState>(StateMachine, OwnerZombie));
	StateMachine->AddState(EZombieStateType::Alert, MakeShared<FZombieAlertState>(StateMachine, OwnerZombie));
	StateMachine->AddState(EZombieStateType::Chase, MakeShared<FZombieChaseState>(StateMachine, OwnerZombie));
	StateMachine->AddState(EZombieStateType::Attack, MakeShared<FZombieAttackState>(StateMachine, OwnerZombie));
	// StateMachine->AddState(EZombieStateType::Stun, MakeShared<FZombieStunState>(StateMachine, Owner));
	StateMachine->AddState(EZombieStateType::Die, MakeShared<FZombieDieState>(StateMachine, OwnerZombie));
	
	StateMachine->ChangeState(EZombieStateType::Idle);
}

EZombieStateType AZombieAIComponent::GetCurrentState() const
{
	if (StateMachine.IsValid())
	{
		return StateMachine->GetCurrentState();
	}
	return EZombieStateType::None;
}

void AZombieAIComponent::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		const TObjectPtr<ACharacterPlayer> TargetPlayer = Cast<ACharacterPlayer>(Actor);
		CurrentTargetCharacter = TargetPlayer;
		
		if (GetCurrentState() == EZombieStateType::Idle || GetCurrentState() == EZombieStateType::Wander)
		{
			StateMachine->ChangeState(EZombieStateType::Alert, TargetPlayer);
		}
	}
	else
	{
		StateMachine->ChangeState(EZombieStateType::Wander);
	}
}
