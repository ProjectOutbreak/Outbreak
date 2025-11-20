// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "PaperSprite.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/CapsuleComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/Component/EquipmentController.h"
#include "Outbreak/Data/PlayerControlData.h"
#include "Outbreak/Game/Controller/InGamePlayerController.h"
#include "Outbreak/Game/Equipment/Weapon/M4.h"
#include "Outbreak/Game/Equipment/Weapon/Knife.h"
#include "Outbreak/Game/Equipment/Weapon/WeaponBase.h"
#include "Outbreak/Game/Framework/InGameMode.h"
#include "Outbreak/Game/Framework/InGameState.h"
#include "Outbreak/Manager/CharacterSpawnManager.h"
#include "Outbreak/Public/Utilities/DebugHelper.h"
#include "Outbreak/UI/InGameHUD.h"

ACharacterPlayer::ACharacterPlayer()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(RootComponent);
	PostProcessComponent->bEnabled = true;
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ToxicAuraMaterialRef(TEXT("/Game/Art/VFX/PostProcess/M_ToxicAura.M_ToxicAura"));
	if (ToxicAuraMaterialRef.Succeeded())
	{
		ToxicAuraPostProcessMaterial = ToxicAuraMaterialRef.Object;
	}
	
	// TODO : for test. delete later
	static ConstructorHelpers::FClassFinder<AM4> WeaponClassRef(TEXT("/Game/Blueprints/BP_M4.BP_M4_C"));
	if (WeaponClassRef.Class)
	{
		WeaponToSpawn = WeaponClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<AKnife> KnifeClassRef(TEXT("/Game/Blueprints/BP_Knife.BP_Knife_C"));
	if (KnifeClassRef.Class)
	{
		KnifeToSpawn = KnifeClassRef.Class;
	}
	
	CharacterType = ECharacterType::Player;
	PlayerType = EPlayerType::Player1;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	EquipmentController = CreateDefaultSubobject<UEquipmentController>(TEXT("EquipmentController"));

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
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMesh(TEXT("/Game/Art/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin"));
	if (DefaultMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(DefaultMesh.Object);
	}
	
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/ABP_Player.ABP_Player_C'"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
	
	// ----- Input
	static ConstructorHelpers::FObjectFinder<UPlayerControlData> FirstPersonDataRef(TEXT("/Script/Outbreak.PlayerControlData'/Game/Data/DA_FirstPersonView.DA_FirstPersonView'"));
	if (FirstPersonDataRef.Object)
	{
		PlayerControlMap.Add(EPlayerControlType::FirstPersonView, FirstPersonDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UPlayerControlData> TopViewDataRef(TEXT("/Script/Outbreak.PlayerControlData'/Game/Data/DA_TopView.DA_TopView'"));
	if (TopViewDataRef.Object)
	{
		PlayerControlMap.Add(EPlayerControlType::TopView, TopViewDataRef.Object);
	}
	
	CurrentCharacterControlType = EPlayerControlType::FirstPersonView;
}

void ACharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerData);
	DOREPLIFETIME(ThisClass, PlayerType);
}

void ACharacterPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if WITH_EDITOR
	SetFolderPath(TEXT("Players"));
#endif
}

void ACharacterPlayer::InitCharacterData()
{
	Super::InitCharacterData();
	
	if (HasAuthority())
	{
		const AInGameMode* GameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
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

		PlayerData = *SpawnManager->GetPlayerData(PlayerType);
	}
	
	CurrentHealth = PlayerData.MaxHealth;
	CurrentExtraHealth = 0;
}

void ACharacterPlayer::UpdateToxicAuraEffect(float Intensity)
{
	if (!ToxicAuraPostProcessMaterial) return;

	if (!ToxicAuraMID)
	{
		ToxicAuraMID = UMaterialInstanceDynamic::Create(ToxicAuraPostProcessMaterial, this);
	}

	ToxicAuraMID->SetScalarParameterValue("EffectIntensity", Intensity);
	PostProcessComponent->Settings.AddBlendable(ToxicAuraMID, FMath::Clamp(Intensity, 0.0f, 1.0f));
}

void ACharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocallyControlled())
	{
		const FString DebugMsg = FString::Printf(TEXT("Player Name : %s"), *GetName());
		PRINT_WITH_CURRENT_CONTEXT(DebugMsg);
		
		SetPlayerControl(CurrentCharacterControlType);
		
		if (const AInGamePlayerController* PC = Cast<AInGamePlayerController>(GetController()))
		{
			CachedHUD = Cast<AInGameHUD>(PC->GetHUD());
			if (!CachedHUD)
			{
				UE_LOG(LogTemp, Warning, TEXT("[%s] Failed to cast HUD"), CURRENT_CONTEXT);
			}
		}
	}

	// TODO : For Test. Remove later.
	if (HasAuthority() && IsValid(WeaponToSpawn) && IsValid(KnifeToSpawn))
	{
		const FVector SpawnLocation = GetActorLocation();
		const FRotator SpawnRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
		KnifeWeapon = GetWorld()->SpawnActor<AWeaponBase>(KnifeToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

		if (IsValid(SpawnedWeapon) && IsValid(KnifeWeapon))
		{
			EquipmentController->AddEquipment(SpawnedWeapon);
			EquipmentController->AddEquipment(KnifeWeapon);
		}
	}
}

void ACharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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

void ACharacterPlayer::ChangePlayerControl()
{
	if (CurrentCharacterControlType == EPlayerControlType::FirstPersonView)
	{
		SetPlayerControl(EPlayerControlType::TopView);
	}
	else if (CurrentCharacterControlType == EPlayerControlType::TopView)
	{
		SetPlayerControl(EPlayerControlType::FirstPersonView);
	}
}

void ACharacterPlayer::SetPlayerControl(EPlayerControlType InPlayerControlType)
{
	const UPlayerControlData* NewCharacterControl = PlayerControlMap[InPlayerControlType];
	if (!NewCharacterControl)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] PlayerControlData not found for type: %d"), CURRENT_CONTEXT, static_cast<int32>(InPlayerControlType));
		return;
	}

	SetPlayerControlData(NewCharacterControl);

	const APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ClearAllMappings();
		const UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentCharacterControlType = InPlayerControlType;
}

void ACharacterPlayer::SetPlayerControlData(const UPlayerControlData* InPlayerControlData)
{
	bUseControllerRotationYaw = InPlayerControlData->bUseControllerRotationYaw;

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	MovementComp->bOrientRotationToMovement = InPlayerControlData->bOrientRotationToMovement;
	MovementComp->bUseControllerDesiredRotation = InPlayerControlData->bUseControllerDesiredRotation;
	MovementComp->RotationRate = InPlayerControlData->RotationRate;

	CameraBoom->TargetArmLength = InPlayerControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(InPlayerControlData->RelativeRotation);
	CameraBoom->SetRelativeLocation(InPlayerControlData->RelativeLocation);
	CameraBoom->bUsePawnControlRotation = InPlayerControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = InPlayerControlData->bInheritPitch;
	CameraBoom->bInheritYaw = InPlayerControlData->bInheritYaw;
	CameraBoom->bInheritRoll = InPlayerControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = InPlayerControlData->bDoCollisionTest;
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

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	MovementComp->bOrientRotationToMovement = false;
	MovementComp->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	MovementComp->MaxStepHeight = 50.f;
	MovementComp->SetWalkableFloorAngle(55.f);
	MovementComp->bUseControllerDesiredRotation = true;
}
