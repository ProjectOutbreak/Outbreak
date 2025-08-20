// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingManager.h"

#include "Kismet/GameplayStatics.h"
#include "Outbreak/Game/Framework/OBGameInstance.h"


void ALoadingManager::BeginPlay()
{
	Super::BeginPlay();

	if (UOBGameInstance* GI = Cast<UOBGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GI->BeginLoading();
	}
}



