// Fill out your copyright notice in the Description page of Project Settings.

#include "LoadingManager.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Game/Framework/OutbreakGameInstance.h"

void ALoadingManager::BeginPlay()
{
	Super::BeginPlay();

	if (UOutbreakGameInstance* GI = Cast<UOutbreakGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GI->BeginLoading();
	}
}