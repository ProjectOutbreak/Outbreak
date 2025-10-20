// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

class UButton;

UCLASS()
class OUTBREAK_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnClickCreateGameButton();
	UFUNCTION()
	void OnClickJoinGameButton();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateGameButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinGameButton;
};