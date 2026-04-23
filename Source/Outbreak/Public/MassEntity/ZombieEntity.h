// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieEntityBase.h"
#include "ZombieEntity.generated.h"

class UMassAgentComponent;

UCLASS()
class OUTBREAK_API AZombieEntity : public AZombieEntityBase
{
	GENERATED_BODY()

public:
	
	/** Default constructor */
	AZombieEntity();

protected:
	
	void OnMassActorActivated();
	
	void OnMassActorDeactivated();
	
protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMassAgentComponent> MassAgentComponent;
	
private:
	
	void OnEntityAssociated(const UMassAgentComponent& AgentComponent);
	
	void OnEntityDetaching(const UMassAgentComponent& AgentComponent);

	void SyncMassToActor();
	
	void SyncActorToMass();

	
public:
	
	// ~ Begin AActor interface
	virtual void BeginPlay() override;
	// ~ End AActor interface
	
protected:
	
	// ~ Begin AZombieEntityBase Interface
	virtual void OnRep_IsDead() override;
	// ~ End AZombieEntityBase Interface
	
};
