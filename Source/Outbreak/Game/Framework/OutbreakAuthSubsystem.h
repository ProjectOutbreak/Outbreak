// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h" 
#include "OutbreakAuthSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class OUTBREAK_API UOutbreakAuthSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = "Auth")
	FString GetSteamId() const;

	UFUNCTION(BlueprintCallable, Category = "Auth")
	FString GetSteamAuthTicket();
private:
	IOnlineIdentityPtr SteamIdentityInterface;
};
