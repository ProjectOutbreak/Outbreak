// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PostProcessComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/Component/CharacterUIComponent.h"
#include "Outbreak/Component/EquipmentController.h"
#include "Outbreak/Data/PlayerControlData.h"
#include "Outbreak/Game/Controller/InGamePlayerController.h"
#include "Outbreak/Game/Equipment/Weapon/M4.h"
#include "Outbreak/Game/Equipment/Weapon/Knife.h"
#include "Outbreak/Game/Equipment/Weapon/Granade.h"
#include "Outbreak/Game/Equipment/Medicine/FirstAidKit.h"
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
	
	// TODO : for test. delete later
	static ConstructorHelpers::FClassFinder<AM4> WeaponClassRef(TEXT("/Game/Blueprints/Equipment/BP_M4.BP_M4_C"));
	if (WeaponClassRef.Class)
	{
		WeaponToSpawn = WeaponClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<AKnife> KnifeClassRef(TEXT("/Game/Blueprints/Equipment/BP_Knife.BP_Knife_C"));
	if (KnifeClassRef.Class)
	{
		KnifeToSpawn = KnifeClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<AGranade> GrenadeClassRef(TEXT("/Game/Blueprints/Equipment/BP_Granade.BP_Granade_C"));
	if (GrenadeClassRef.Class)
	{
		GrenadeToSpawn = GrenadeClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<AEquipmentBase> HealClassRef(TEXT("/Game/Blueprints/Equipment/BP_FirstAidKit.BP_FirstAidKit_C"));
	if (HealClassRef.Class)
	{
		HealToSpawn = HealClassRef.Class;
	}
	
	CharacterType = ECharacterType::Player;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	EquipmentController = CreateDefaultSubobject<UEquipmentController>(TEXT("EquipmentController"));

	UIComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("UIComponent"));
	UIComponent->SetupAttachment(RootComponent);
	UIComponent->SetChildActorClass(ACharacterUIComponent::StaticClass());
	UIComponent->SetRelativeLocation(FVector::ZeroVector);
}

void ACharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerData);
	DOREPLIFETIME(ThisClass, PlayerType);
}

void ACharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!CachedController)
	{
		CachedController = NewController;
	}
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

		if (ACharacterUIComponent* UIRig = Cast<ACharacterUIComponent>(UIComponent->GetChildActor()))
		{
			UIRig->SetPlayerName(GetName());
		}
		SetPlayerControl(CurrentCharacterControlType);
		
		if (AInGamePlayerController* PC = Cast<AInGamePlayerController>(GetController()))
		{
			if (PC->PlayerCameraManager)
			{
				PC->PlayerCameraManager->ViewPitchMin = -15.0f; 
				PC->PlayerCameraManager->ViewPitchMax = 20.0f; 
			}
		}
	}

	// TODO : For Test. Remove later.
	if (HasAuthority() && IsValid(WeaponToSpawn) && IsValid(KnifeToSpawn) && IsValid(GrenadeToSpawn) && IsValid(HealToSpawn))
	{
		const FVector SpawnLocation = GetActorLocation();
		const FRotator SpawnRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
		KnifeWeapon = GetWorld()->SpawnActor<AWeaponBase>(KnifeToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
		GrenadeWeapon = GetWorld()->SpawnActor<AWeaponBase>(GrenadeToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
		HealWeapon = GetWorld()->SpawnActor<AEquipmentBase>(HealToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

		if (IsValid(SpawnedWeapon) && IsValid(KnifeWeapon) && IsValid(GrenadeWeapon) && IsValid(HealWeapon))
		{
			EquipmentController->AddEquipment(SpawnedWeapon);
			EquipmentController->AddEquipment(KnifeWeapon);
			EquipmentController->AddEquipment(GrenadeWeapon);
			EquipmentController->AddEquipment(HealWeapon);
		}
		EquipmentController->UnEquipCurrentEquipment();
	}
}

void ACharacterPlayer::Die()
{
	if (!HasAuthority()) return;
	
	AController* SavedController = GetController();
	
	if (AInGameMode* Gm = Cast<AInGameMode>(GetWorld()->GetAuthGameMode()))
	{
		Gm->OnPlayerDie(this, SavedController);
	}
	
	Super::Die();
}

void ACharacterPlayer::OnRep_Die()
{
	Super::OnRep_Die();
	
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetSimulatePhysics(true);
	}
	
	ClearInputMappings();
}

void ACharacterPlayer::OnRep_CurrentHealth()
{
	Super::OnRep_CurrentHealth();
	
	if (const APlayerController* PC = Cast<APlayerController>(CachedController))
	{
		if (AInGameHUD* HUD = Cast<AInGameHUD>(PC->GetHUD()))
		{
			HUD->DisplayCurrentHealth(CurrentHealth);
		}
	}
}

void ACharacterPlayer::OnRep_Controller()
{
	Super::OnRep_Controller();
	
	if (!CachedController)
	{
		CachedController = GetController();
	}
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

	const APlayerController* PlayerController = CastChecked<APlayerController>(CachedController);
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		if (const UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext)
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

void ACharacterPlayer::ClearInputMappings() const
{
	if (IsLocallyControlled())
	{
		if (const APlayerController* PC = Cast<APlayerController>(CachedController))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				Subsystem->ClearAllMappings();
			}
		}
	}
}

void ACharacterPlayer::SetupCollision()
{
	Super::SetupCollision();

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	auto* MeshComp = GetMesh();
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
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

void ACharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
#if !UE_BUILD_SHIPPING
	PlayerInputComponent->BindKey(EKeys::P, IE_Pressed, this, &ThisClass::Server_DebugTakeDamage);
#endif
}

void ACharacterPlayer::Server_DebugTakeDamage_Implementation()
{
	TakeDamage(10.0f, FDamageEvent(), CachedController, this);
}