// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootStepComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OUTBREAK_API UFootStepComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFootStepComponent();

	void HandleFootStep(const FName& SocketName);
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<class USoundManager> SoundManager;

	UPROPERTY()
	TObjectPtr<class ACharacterBase> OwningCharacter;
};
