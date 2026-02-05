// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UEasySessionSubsystem;
class UTextBlock;
class UButton;

UCLASS()
class OUTBREAK_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	// ~ Begin UMG
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_GameStart;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_LobbyCode;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_PlayerList;
	// ~ End UMG
	
private:
	UFUNCTION()
	void OnPlayerListUpdate();
	
	// ~ Begin Button Callbacks
	UFUNCTION()
	void OnClickGameStartButton();
	// ~ End Button Callbacks
	
	UPROPERTY(Transient)
	TObjectPtr<UEasySessionSubsystem> SessionsSubsystem;
	FTimerHandle PlayerListTimerHandle;
};
