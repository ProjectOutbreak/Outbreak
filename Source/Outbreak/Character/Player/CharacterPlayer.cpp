// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "PaperSprite.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/Game/Framework/OBGameMode.h"
#include "Outbreak/Game/Framework/OutBreakGameState.h"
#include "Outbreak/Manager/CharacterSpawnManager.h"

ACharacterPlayer::ACharacterPlayer()
{
	CharacterType = ECharacterType::Player;
	PlayerType = EPlayerType::Player1;

	// ----- Camara Component
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera -> SetupAttachment(RootComponent);
	FirstPersonCamera -> SetRelativeLocation(FVector(0, 0, BaseEyeHeight));
	FirstPersonCamera -> SetWorldRotation(FRotator(0, 90.0f, 0));
	FirstPersonCamera -> bUsePawnControlRotation = true;

	TopViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopViewCamera"));
	TopViewCamera->SetupAttachment(RootComponent);
	TopViewCamera->SetRelativeLocation(FVector(0.f, 0.f, 800.f));
	TopViewCamera->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	TopViewCamera->bUsePawnControlRotation = false;

	// ----- MiniMap
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture2D"));
	SceneCapture->ProjectionType = ECameraProjectionMode::Type::Orthographic;
	SceneCapture->OrthoWidth = 4000.f;
	SceneCapture->SetupAttachment(RootComponent); 
	SceneCapture->SetRelativeLocation(FVector(0.f, 0.f, 2100.f)); 
	SceneCapture->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); 
	SceneCapture->bCaptureEveryFrame = true;
	SceneCapture->bCaptureOnMovement = false;
	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetRef(TEXT("/Script/Engine.TextureRenderTarget2D'/Game/Art/UI/MiniMap/RT_MiniMap.RT_MiniMap'"));
	if (RenderTargetRef.Succeeded())
	{
		SceneCapture->TextureTarget = RenderTargetRef.Object;
	}

	// ----- UI
	PlayerIconSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PlayerIconSprite"));
	PlayerIconSprite->SetupAttachment(GetCapsuleComponent());
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
	PlayerNameText->SetupAttachment(GetCapsuleComponent());
	PlayerNameText->SetRelativeLocation(FVector(-250.f, 0.f, 2000.f));
	PlayerNameText->SetRelativeRotation(FRotator(90.f, 180.f, 0.f));
	PlayerNameText->SetHorizontalAlignment(EHTA_Center);
	PlayerNameText->SetVerticalAlignment(EVRTA_TextCenter);
	PlayerNameText->SetWorldSize(200.f); 
	PlayerNameText->SetTextRenderColor(FColor::White);
	PlayerNameText->SetVisibility(true);
	PlayerNameText->bVisibleInSceneCaptureOnly = true;
	
	// ----- Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Art/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
	if (DefaultMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(DefaultMesh.Object);
	}
	
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Script/Engine.AnimBlueprint'/Game/Art/Characters/Mannequins/Animations/ABP_Manny.ABP_Manny_C'"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
	
	// ----- Input
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Inputs/IMC_Player.IMC_Player'"));
	if (InputMappingContextRef.Object)
	{
		InputMappingContext = InputMappingContextRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionChangeCamRef(TEXT("/Script/EnhancedInputComponent.InputAction'/Game/Inputs/IA_ChangePerspective.IA_ChangePerspective'"));
	if (InputActionChangeCamRef.Object)
	{
		ChangeCameraAction = InputActionChangeCamRef.Object;
	}
	
	CurrentCharacterControlType = EPlayerControlType::Top;
}

void ACharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterPlayer, PlayerData);
	DOREPLIFETIME(ACharacterPlayer, PlayerType);
}

void ACharacterPlayer::InitCharacterData()
{
	Super::InitCharacterData();
	
	if (HasAuthority())
	{
		const AOBGameMode * GameMode = Cast<AOBGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (!GameMode)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] GameMode is null!"), CURRENT_CONTEXT);
			return;
		}
		ACharacterSpawnManager* SpawnManager = GameMode->GetSpawnManager();
		if (!SpawnManager)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] SpawnManager is null!"), CURRENT_CONTEXT);
			return;
		}

		const FPlayerData* Data = SpawnManager->GetPlayerData(PlayerType);
		PlayerData = *Data;
		SpawnManager->Activate(this);
	}
	
	CurrentHealth = PlayerData.MaxHealth;
	CurrentExtraHealth = 0;
}

void ACharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->SetOwnerNoSee(true);

	if (IsLocallyControlled())
	{
		SetCharacterControl(CurrentCharacterControlType);
		if (FirstPersonCamera)
		{
			FirstPersonCamera->SetActive(true);
		}
		if (TopViewCamera)
		{
			TopViewCamera->SetActive(false);
		}
	}
}

void ACharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInputComponent->BindAction(ChangeCameraAction, ETriggerEvent::Triggered, this, &ACharacterPlayer::ToggleCameraMode);
}

void ACharacterPlayer::OnRep_Die()
{
	Super::OnRep_Die();
	
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetSimulatePhysics(true);
	}
	
	DetachFromControllerPendingDestroy();
}

void ACharacterPlayer::ToggleCameraMode()
{
	if (CurrentCameraMode == ECameraMode::FPS)
	{
		CurrentCameraMode = ECameraMode::TopView;
		FirstPersonCamera->SetActive(false);
		FirstPersonCamera -> SetRelativeRotation(FRotator(-10.f,0.f,0.f));
		GetMesh()->SetOwnerNoSee(false);
		TopViewCamera->SetActive(true);
	}
	else
	{
		CurrentCameraMode = ECameraMode::FPS;
		TopViewCamera->SetActive(false);
		GetMesh()->SetOwnerNoSee(true);
		FirstPersonCamera->SetActive(true);
	}
}

void ACharacterPlayer::SetCharacterControl(EPlayerControlType NewCharacterControlType)
{
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
	CurrentCharacterControlType = NewCharacterControlType;
}

void ACharacterPlayer::SetupCollision()
{
	Super::SetupCollision();

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	auto* MeshComp = GetMesh();
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	MeshComp->bOwnerNoSee = true;
	// MeshComp->SetHiddenInGame(true);
}

void ACharacterPlayer::SetupMovement()
{
	Super::SetupMovement();

	auto* MovementComp = GetCharacterMovement();
	MovementComp->MaxStepHeight = 50.f;
	MovementComp->SetWalkableFloorAngle(55.f);
	MovementComp->bUseControllerDesiredRotation = true;
}