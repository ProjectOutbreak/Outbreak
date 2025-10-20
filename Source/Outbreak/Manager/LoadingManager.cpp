// Fill out your copyright notice in the Description page of Project Settings.

#include "LoadingManager.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Game/Framework/OutBreakGameInstance.h"

void ALoadingManager::BeginPlay()
{
	Super::BeginPlay();

	if (UOutBreakGameInstance* GI = Cast<UOutBreakGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GI->BeginLoading();
	}
}