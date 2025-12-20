// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterUIComponent.generated.h"

UCLASS()
class OUTBREAK_API ACharacterUIComponent : public AActor
{
	GENERATED_BODY()
	
public:	
	ACharacterUIComponent();

protected:
	virtual void BeginPlay() override;

public:	
	void SetPlayerName(const FString& Name);


protected:
	UPROPERTY(VisibleAnywhere, Category = "MiniMap")
	TObjectPtr<class USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, Category = "MiniMap")
	TObjectPtr<class USceneCaptureComponent2D> SceneCapture;

	UPROPERTY(VisibleAnywhere, Category = "MiniMap")
	TObjectPtr<class UPaperSpriteComponent> PlayerIconSprite;

	UPROPERTY(VisibleAnywhere, Category = "MiniMap")
	TObjectPtr<class UTextRenderComponent> PlayerNameText;
};
