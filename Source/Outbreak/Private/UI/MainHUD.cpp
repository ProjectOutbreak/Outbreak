// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/MainWidget.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidgetClass)
	{
		if (APlayerController* PC = GetOwningPlayerController())
		{
			MainWidgetInstance = CreateWidget<UMainWidget>(PC, MainWidgetClass);

			if (MainWidgetInstance)
			{
				MainWidgetInstance->AddToViewport();

				FInputModeUIOnly InputModeData;
				InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                
				PC->SetInputMode(InputModeData);
				PC->SetShowMouseCursor(true);
			}
		}
	}
}

void AMainHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (MainWidgetInstance)
	{
		MainWidgetInstance->RemoveFromParent();
		MainWidgetInstance = nullptr;
	}

	if (APlayerController* PC = GetOwningPlayerController())
	{
		const FInputModeGameOnly InputModeData;
		PC->SetInputMode(InputModeData);
		PC->SetShowMouseCursor(false);
	}
}
