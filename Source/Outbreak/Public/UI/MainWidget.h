// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

class UEasySessionSubsystem;
class UWidgetSwitcher;
class UTextBlock;
class UEditableTextBox;
class UButton;

UCLASS()
class OUTBREAK_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	// ~ Begin UMG
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_SinglePlay;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_CreateLobby;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_JoinLobby;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UEditableTextBox> ETB_LobbyCode;
	
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadWrite)
	TObjectPtr<UWidgetSwitcher> WS_Main;

	UPROPERTY(meta = (BindWidgetOptional)) 
	class UCheckBox* CheckBox_UseDedicated;
	// ~ End UMG
	
private:
	void SetButtonsEnabled(bool BNewIsEnabled);
	FString GenerateRandomLobbyCode(int32 Length);

	// ~ Begin Session Subsystem
	void BindSessionSubsystemCallbacks();
	void RemoveSessionSubsystemCallbacks();
	
	void OnStartSessionFailure();
	void OnFindSessionSuccess(const TArray<FOnlineSessionSearchResult>& SessionResults);
	void OnFindSessionFailure(const TArray<FOnlineSessionSearchResult>& SessionResults);
	void OnJoinSessionSuccess();
	void OnJoinSessionFailure();
	void OnDestroySessionSuccess();
	void OnDestroySessionFailure();
	// ~ End Session Subsystem
	
	
	// ~ Begin Button Callbacks
	UFUNCTION()
	void OnClickCreateGameButton();
	UFUNCTION()
	void OnClickJoinGameButton();
	UFUNCTION()
	void OnClickSinglePlayButton();
	// ~ End Button Callbacks
	
	UPROPERTY(Transient)
	TObjectPtr<UEasySessionSubsystem> SessionsSubsystem;
	
	FString CachedCreatedLobbyCode;
};