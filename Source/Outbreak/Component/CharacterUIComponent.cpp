#include "CharacterUIComponent.h"

#include "CharacterUIComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextRenderComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "Engine/TextureRenderTarget2D.h"

ACharacterUIComponent::ACharacterUIComponent()
{
	PrimaryActorTick.bCanEverTick = false;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture2D"));
    SceneCapture->SetupAttachment(RootComponent);
    SceneCapture->ProjectionType = ECameraProjectionMode::Type::Orthographic;
    SceneCapture->OrthoWidth = 4000.f;
    SceneCapture->SetRelativeLocation(FVector(0.f, 0.f, 2100.f)); 
    SceneCapture->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); 
    SceneCapture->bCaptureEveryFrame = false;
    SceneCapture->bCaptureOnMovement = false;

    static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetRef(TEXT("/Script/Engine.TextureRenderTarget2D'/Game/Art/UI/MiniMap/RT_MiniMap.RT_MiniMap'"));
    if (RenderTargetRef.Succeeded())
    {
       SceneCapture->TextureTarget = RenderTargetRef.Object;
    }

    PlayerIconSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PlayerIconSprite"));
    PlayerIconSprite->SetupAttachment(RootComponent);
    PlayerIconSprite->SetRelativeLocation(FVector(0.f, 0.f, 2000.f));
    PlayerIconSprite->SetRelativeRotation(FRotator(-180.f, -180.f, -90.f));
    PlayerIconSprite->SetRelativeScale3D(FVector(0.5f));       
    PlayerIconSprite->SetVisibility(true);
    PlayerIconSprite->bVisibleInSceneCaptureOnly = true;

    static ConstructorHelpers::FObjectFinder<UPaperSprite> PlayerIconAsset(TEXT("/Script/Paper2D.PaperSprite'/Game/Art/UI/MiniMap/PlayerIcon_Sprite.PlayerIcon_Sprite'"));
    if (PlayerIconAsset.Succeeded())
    {
       PlayerIconSprite->SetSprite(PlayerIconAsset.Object);
    }

    PlayerNameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PlayerNameText"));
    PlayerNameText->SetupAttachment(RootComponent);
    PlayerNameText->SetRelativeLocation(FVector(-250.f, 0.f, 2000.f));
    PlayerNameText->SetRelativeRotation(FRotator(90.f, 180.f, 0.f));
    PlayerNameText->SetHorizontalAlignment(EHTA_Center);
    PlayerNameText->SetVerticalAlignment(EVRTA_TextCenter);
    PlayerNameText->SetWorldSize(200.f); 
    PlayerNameText->SetTextRenderColor(FColor::White);
    PlayerNameText->SetVisibility(true);
    PlayerNameText->bVisibleInSceneCaptureOnly = true;
}

void ACharacterUIComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterUIComponent::SetPlayerName(const FString& Name)
{
    if (PlayerNameText)
    {
        PlayerNameText->SetText(FText::FromString(Name));
    }
}
void ACharacterUIComponent::SetSceneCaptureActive(bool bActive)
{
	if (SceneCapture)
	{
		SceneCapture->SetActive(bActive);
		SceneCapture->bCaptureEveryFrame = false;

		if (bActive)
		{
			GetWorldTimerManager().SetTimer(
				MinimapCaptureTimer,
				this,
				&ACharacterUIComponent::UpdateMinimap,
				0.033f,
				true
			);
		}
		else
		{
			GetWorldTimerManager().ClearTimer(MinimapCaptureTimer);
		}
	}
}
void ACharacterUIComponent::HidePlayerIcon()
{
	if (PlayerIconSprite)
	{
		PlayerIconSprite->SetVisibility(false);
	}
	if (PlayerNameText)
	{
		PlayerNameText->SetVisibility(false);
	}
}
void ACharacterUIComponent::UpdateMinimap()
{
	if (SceneCapture && SceneCapture->IsActive())
	{
		SceneCapture->CaptureScene();
	}
}