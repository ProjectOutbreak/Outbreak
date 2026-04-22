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
#include "Outbreak/Game/Equipment/Weapon/WeaponBase.h"
#include "Outbreak/Game/Framework/InGameMode.h"
#include "Outbreak/Game/Framework/InGameState.h"
#include "Outbreak/UI/InGameHUD.h"
#include "Outbreak/Util/DataTableHelper.h"
#include "Outbreak/UI/OBWidget.h"
ACharacterPlayer::ACharacterPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(RootComponent);
	PostProcessComponent->bEnabled = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	EquipmentController = CreateDefaultSubobject<UEquipmentController>(TEXT("EquipmentController"));
}

void ACharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled() && CurrentCharacterControlType == EPlayerControlType::FirstPersonView)
	{
		FRotator ControlRot = GetControlRotation();
		float Pitch = ControlRot.Pitch;
		
		if (Pitch > 180.0f) Pitch -= 360.0f;

		float TargetX = DefaultCameraX;
		if (Pitch < 0.0f) 
		{
			float Alpha = FMath::Clamp(FMath::Abs(Pitch) / 45.0f, 0.0f, 1.0f);
    
			TargetX = FMath::Lerp(DefaultCameraX, AimDownCameraX, Alpha);
		}
		FVector CurrentLoc = FollowCamera->GetRelativeLocation();
		float NewX = FMath::FInterpTo(CurrentLoc.X, TargetX, DeltaTime, 10.0f);

		FollowCamera->SetRelativeLocation(FVector(NewX, CurrentLoc.Y, CurrentLoc.Z));
	}
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
	
	DataTableHelper::LoadDataTableToMap(PlayerDataTable, PlayerDataMap);
	
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

float ACharacterPlayer::GetHealthRatio() const
{
	return (float)CurrentHealth / PlayerData.MaxHealth;
}

void ACharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	if (ACharacterUIComponent* SpawnedUIComponent = GetWorld()->SpawnActor<ACharacterUIComponent>(ACharacterUIComponent::StaticClass(), GetActorLocation(),	GetActorRotation(), SpawnParams))
	{
		SpawnedUIComponent->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SpawnedUIComponent->SetPlayerName(GetName());
		SpawnedUIComponent->SetSceneCaptureActive(IsLocallyControlled());
		UIComponent = SpawnedUIComponent;
	}
	
	if (IsLocallyControlled())
	{
		SetPlayerControl(CurrentCharacterControlType);
		SetInitialStateUI();
		if (AInGamePlayerController* PC = Cast<AInGamePlayerController>(GetController()))
		{
			if (PC->PlayerCameraManager)
			{
				PC->PlayerCameraManager->ViewPitchMin = -45.0f; 
				PC->PlayerCameraManager->ViewPitchMax = 20.0f; 
			}
		}
	}
}

void ACharacterPlayer::Die()
{
	if (!HasAuthority()) return;

	Multicast_HidePlayerIcon();
	
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
void ACharacterPlayer::Multicast_HidePlayerIcon_Implementation()
{
	if (UIComponent)
	{
		UIComponent->HidePlayerIcon();
	}
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
	if (APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (AInGameHUD* LocalHUD = Cast<AInGameHUD>(LocalPC->GetHUD()))
		{
			if (UOBWidget* Widget = LocalHUD->GetInGameWidget())
			{
				float Ratio = (float)CurrentHealth / 100.0f;
				Widget->UpdateMemberHealth(GetPlayerState(), Ratio);
			}
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
	if (IsLocallyControlled())
	{
		SetInitialStateUI();
	}
}

void ACharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (const APlayerController* PC = Cast<APlayerController>(CachedController))
	{
		if (AInGameHUD* HUD = Cast<AInGameHUD>(PC->GetHUD()))
		{
			HUD->DisplayMembers();
		}
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

void ACharacterPlayer::SetInitialStateUI()
{
	OnRep_CurrentHealth();
}

void ACharacterPlayer::SetupMovement()
{
	Super::SetupMovement();

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->bOrientRotationToMovement = false;
		MovementComp->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
		MovementComp->MaxStepHeight = 50.f;
		MovementComp->SetWalkableFloorAngle(55.f);
		MovementComp->bUseControllerDesiredRotation = true;
	}
}

void ACharacterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (EndPlayReason == EEndPlayReason::LevelTransition || EndPlayReason == EEndPlayReason::Quit)
        {
            Super::EndPlay(EndPlayReason);
            return;
        }
		
		if (EndPlayReason == EEndPlayReason::Destroyed || EndPlayReason == EEndPlayReason::RemovedFromWorld)
		{
			if (EquipmentController)
			{
				EquipmentController->DestroyAllEquipment();
			}
			OnCharacterDeathDelegate.Broadcast(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ACharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
#if !UE_BUILD_SHIPPING
	PlayerInputComponent->BindKey(EKeys::P, IE_Pressed, this, &ThisClass::Server_DebugTakeDamage);
	// Test : Call GameOver by key input "L"
	InputComponent->BindKey(EKeys::L, IE_Pressed, this, &ThisClass::Input_RequestGameOver);
#endif
}

void ACharacterPlayer::Server_DebugTakeDamage_Implementation()
{
	TakeDamage(10.0f, FDamageEvent(), CachedController, this);
}

void ACharacterPlayer::Server_PickupEquipment_Implementation(class AEquipmentBase* NewEquipment)
{
	if (EquipmentController && IsValid(NewEquipment))
	{
		EquipmentController->PickupEquipment(NewEquipment);
	}
}

void ACharacterPlayer::Input_RequestGameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("Key Pressed: Requesting Game Over..."));
	Server_RequestGameOver();
}

void ACharacterPlayer::Server_RequestGameOver_Implementation()
{
	if (AInGameMode* GM = Cast<AInGameMode>(GetWorld()->GetAuthGameMode()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Server] Admin Command Received: Game Over!"));
		GM->GameOver(); 
	}
}
