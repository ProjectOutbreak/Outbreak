// Fill out your copyright notice in the Description page of Project Settings.

#include "InGamePlayerController.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Outbreak/UI/InGameHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/UI/OBWidget.h"
#include "Utilities/DebugHelper.h"

AInGamePlayerController::AInGamePlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFirstPersonMoveRef(TEXT("/Game/Inputs/Actions/IA_FirstPersonMove.IA_FirstPersonMove"));
	if (InputActionFirstPersonMoveRef.Object)
	{
		FirstPersonMoveAction = InputActionFirstPersonMoveRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFirstPersonLookRef(TEXT("/Game/Inputs/Actions/IA_FirstPersonLook.IA_FirstPersonLook"));
	if (InputActionFirstPersonLookRef.Object)
	{
		FirstPersonLookAction = InputActionFirstPersonLookRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionTopMoveRef(TEXT("/Game/Inputs/Actions/IA_TopMove.IA_TopMove"));
	if (InputActionTopMoveRef.Object)
	{
		TopMoveAction = InputActionTopMoveRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Game/Inputs/Actions/IA_Jump.IA_Jump"));
	if (InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSprintRef(TEXT("/Game/Inputs/Actions/IA_Sprint.IA_Sprint"));
	if (InputActionSprintRef.Object)
	{
		SprintAction = InputActionSprintRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionCrouchRef(TEXT("/Game/Inputs/Actions/IA_Crouch.IA_Crouch"));
	if (InputActionCrouchRef.Object)
	{
		CrouchAction = InputActionCrouchRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionChangeCamRef(TEXT("/Game/Inputs/Actions/IA_ChangeControl.IA_ChangeControl"));
	if (InputActionChangeCamRef.Object)
	{
		ChangePlayerControlAction = InputActionChangeCamRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionToggleMenuRef(TEXT("/Game/Inputs/Actions/IA_ToggleMenu.IA_ToggleMenu"));
	if (InputActionToggleMenuRef.Object)
	{
		ToggleMenuAction = InputActionToggleMenuRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFireRef(TEXT("/Game/Inputs/Actions/IA_Use.IA_Use"));
	if (InputActionFireRef.Object)
	{
		UseAction = InputActionFireRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionEndFireRef(TEXT("/Game/Inputs/Actions/IA_EndUse.IA_EndUse"));
	if (InputActionEndFireRef.Object)
	{
		EndUseAction = InputActionEndFireRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionReloadRef(TEXT("/Game/Inputs/Actions/IA_Reload.IA_Reload"));
	if (InputActionReloadRef.Object)
	{
		ReloadAction = InputActionReloadRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSelectEquipmentRef(TEXT("/Game/Inputs/Actions/IA_SelectEquipment.IA_SelectEquipment"));
	if (InputActionSelectEquipmentRef.Object)
	{
		SelectEquipmentAction = InputActionSelectEquipmentRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionToggleFireModeRef(TEXT("/Game/Inputs/Actions/IA_ToggleFireMode.IA_ToggleFireMode"));
	if (InputActionToggleFireModeRef.Object)
	{
		ToggleFireModeAction = InputActionToggleFireModeRef.Object;
	}
}

void AInGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		const FString DebugMsg = FString::Printf(TEXT("Is Server: %s"), HasAuthority() ? TEXT("True") : TEXT("False"));
		PRINT_WITH_CURRENT_CONTEXT(DebugMsg);
	}
}

void AInGamePlayerController::OnPossess(class APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);

	ControlledCharacter = Cast<ACharacterPlayer>(PossessedPawn);
	if (!ControlledCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] PossessedPawn is not a CharacterPlayer!"), CURRENT_CONTEXT);
	}
}

void AInGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(FirstPersonMoveAction, ETriggerEvent::Triggered, this, &AInGamePlayerController::FirstPersonMove);
	EnhancedInputComponent->BindAction(FirstPersonLookAction, ETriggerEvent::Triggered, this, &AInGamePlayerController::FirstPersonLook);
	EnhancedInputComponent->BindAction(TopMoveAction, ETriggerEvent::Triggered, this, &AInGamePlayerController::TopMove);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AInGamePlayerController::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AInGamePlayerController::StopJumping);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AInGamePlayerController::Run);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AInGamePlayerController::StopRun);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AInGamePlayerController::Crouch);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AInGamePlayerController::StopCrouch);
	EnhancedInputComponent->BindAction(ChangePlayerControlAction, ETriggerEvent::Triggered, this, &AInGamePlayerController::ChangePlayerControl);
	EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started, this, &AInGamePlayerController::TogglePauseMenu);
	EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Started, this, &AInGamePlayerController::Use);
	EnhancedInputComponent->BindAction(EndUseAction, ETriggerEvent::Completed, this, &AInGamePlayerController::EndUse);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AInGamePlayerController::Reload);
	EnhancedInputComponent->BindAction(SelectEquipmentAction, ETriggerEvent::Triggered, this, &AInGamePlayerController::SelectEquipment);
	EnhancedInputComponent->BindAction(ToggleFireModeAction, ETriggerEvent::Started, this, &AInGamePlayerController::ToggleFireMode);
}

void AInGamePlayerController::FirstPersonMove(const FInputActionValue& Value)
{
	if (bMenuOpen) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	// if (ControlledCharacter->CanMove())
	if (ControlledCharacter && MovementVector != FVector2D::ZeroVector)
	{

		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledCharacter->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AInGamePlayerController::FirstPersonLook(const FInputActionValue& Value)
{
	if (!ControlledCharacter) return;
	
	if (bMenuOpen) return;
	const FVector2D LookAxis = Value.Get<FVector2D>();
	ControlledCharacter->AddControllerYawInput(LookAxis.X);
	ControlledCharacter->AddControllerPitchInput(LookAxis.Y);
}

void AInGamePlayerController::TopMove(const FInputActionValue& Value)
{
	if (!ControlledCharacter) return;
	
	FVector2D MovementVector = Value.Get<FVector2D>();

	float MovementVectorSize = 1.0f;
	const float MovementVectorSizeSquared = MovementVector.SquaredLength();
	if (MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	const FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	ControlledCharacter->GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	ControlledCharacter->AddMovementInput(MoveDirection, MovementVectorSize);
}

void AInGamePlayerController::Jump()
{
	if (!ControlledCharacter || bMenuOpen) return;
	
	ControlledCharacter->Jump();
}

void AInGamePlayerController::StopJumping()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->StopJumping();
}

void AInGamePlayerController::Run()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AInGamePlayerController::StopRun()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AInGamePlayerController::Crouch()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
}

void AInGamePlayerController::StopCrouch()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AInGamePlayerController::ChangePlayerControl()
{
	if (!ControlledCharacter || bMenuOpen) return;
	
	ControlledCharacter->ChangePlayerControl();
}


void AInGamePlayerController::TogglePauseMenu()
{
	bMenuOpen = !bMenuOpen;
	bShowMouseCursor = bMenuOpen;

	if (AInGameHUD* HUD = Cast<AInGameHUD>(GetHUD()))
	{
		if (UOBWidget* W = HUD->GetOBWidget())
		{
			if (bMenuOpen)
			{
				W->ShowPauseMenu(true);
				FInputModeGameAndUI Mode;
				Mode.SetWidgetToFocus(W->TakeWidget());
				Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				Mode.SetHideCursorDuringCapture(false);
				SetInputMode(Mode);
			}
			else
			{
				W->ShowPauseMenu(false);
				SetInputMode(FInputModeGameOnly{});
				UWidgetBlueprintLibrary::SetFocusToGameViewport();
			}
		}
	}
	if (APawn* P = GetPawn())
	{
		if (UCharacterMovementComponent* Move = Cast<UCharacterMovementComponent>(P->GetMovementComponent()))
		{
			if (bMenuOpen)
			{
				Move->StopMovementImmediately();
			}
			else
			{
				Move->SetMovementMode(MOVE_Walking);
				Move->MaxWalkSpeed = WalkSpeed;
			}
		}
	}
}

void AInGamePlayerController::Use()
{
	if (!ControlledCharacter) return;

	ControlledCharacter->HandleUse();
}

void AInGamePlayerController::EndUse()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->HandleEndUse();
}

void AInGamePlayerController::Reload()
{
	if (!ControlledCharacter) return;

	ControlledCharacter->HandleReload();
}

void AInGamePlayerController::SelectEquipment(const FInputActionValue& Value)
{
	const float SlotNumber = Value.Get<float>();
	const int32 SlotIndex = FMath::RoundToInt(SlotNumber);

	UE_LOG(LogTemp, Log, TEXT("[%s] Select Equipment Slot: %d"), CURRENT_CONTEXT, SlotIndex);
}

void AInGamePlayerController::ToggleFireMode()
{
	if (!ControlledCharacter) return;

	ControlledCharacter->HandleToggleFireMode();
}
