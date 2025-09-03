// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OUTBREAK_API IInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(APawn* InstigatorPawn);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction") 
	void BeginFocus(); // 플레이어가 상호작용 대상을 보기 시작했을 때 (UI 띄우기 등)


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	void EndFocus(); // 상호작용 대상에서 시선 돌렸을 때 (UI 없애기 등) 
};
