// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CreateGameButton)	CreateGameButton->OnClicked.AddDynamic(this, &UMainWidget::OnClickCreateGameButton);
	if (JoinGameButton)		JoinGameButton->OnClicked.AddDynamic(this, &UMainWidget::OnClickJoinGameButton);
}

void UMainWidget::OnClickCreateGameButton()
{
	const FString MapName = "L_Loading";
	const FString Options = "?listen";
	
	UGameplayStatics::OpenLevel(this, FName(* (MapName + Options)));
}

void UMainWidget::OnClickJoinGameButton()
{
	const FString IPAddress = "127.0.0.1";

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->ClientTravel(IPAddress, ETravelType::TRAVEL_Absolute);
	}
}