// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MainWidget.generated.h"

class USessionSubsystem;
class UEditableTextBox;
class UButton;

UCLASS()
class OUTBREAK_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	void SetButtonsEnabled(bool BNewIsEnabled);
	const TCHAR* JoinSessionResultToText(EOnJoinSessionCompleteResult::Type InResult) const;
	FString GenerateRandomLobbyCode(int32 Length);

	// ~ Begin Seesion Subsystem
	void BindSessionSubsystemCallbacks();
	void RemoveSessionSubsystemCallbacks();
	
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnSessionError(const FString& Reason);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	// ~ End Session Subsystem
	
	// ~ Begin Button Callbacks
	UFUNCTION()
	void OnClickCreateGameButton();
	UFUNCTION()
	void OnClickJoinGameButton();
	UFUNCTION()
	void OnClickSinglePlayButton();
	// ~ End Button Callbacks
	
	// ~ Begin UMG
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateGameButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinGameButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SinglePlayButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TB_LobbyCode;
	// ~ End UMG
	
	UPROPERTY(Transient)
	TObjectPtr<USessionSubsystem> SessionsSubsystem;
};