// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PerformanceWidget.generated.h"

class UTextBlock;

UCLASS()
class OUTBREAK_API UPerformanceWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_FPS;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Ping;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_MS;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float UpdateInterval = 0.5f;

private:
	void CalculateAndUpdatePerformances(float InDeltaTime);
	float DeltaTimeAccumulator = 0.0f;
	int32 FrameCount = 0;
};