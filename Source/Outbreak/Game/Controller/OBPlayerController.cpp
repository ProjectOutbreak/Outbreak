// Fill out your copyright notice in the Description page of Project Settings.

#include "OBPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Outbreak/UI/OBHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/UI/OBWidget.h"
#include "Outbreak/Util/EnumHelper.h"

AOBPlayerController::AOBPlayerController()
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

void AOBPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AOBPlayerController::OnPossess(class APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);

	ControlledCharacter = Cast<ACharacterPlayer>(PossessedPawn);
	if (!ControlledCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] PossessedPawn is not a CharacterPlayer!"), CURRENT_CONTEXT);
	}
}

void AOBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(FirstPersonMoveAction, ETriggerEvent::Triggered, this, &AOBPlayerController::FirstPersonMove);
	EnhancedInputComponent->BindAction(FirstPersonLookAction, ETriggerEvent::Triggered, this, &AOBPlayerController::FirstPersonLook);
	EnhancedInputComponent->BindAction(TopMoveAction, ETriggerEvent::Triggered, this, &AOBPlayerController::TopMove);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AOBPlayerController::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AOBPlayerController::StopJumping);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AOBPlayerController::Run);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AOBPlayerController::StopRun);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AOBPlayerController::Crouch);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AOBPlayerController::StopCrouch);
	EnhancedInputComponent->BindAction(ChangePlayerControlAction, ETriggerEvent::Triggered, this, &AOBPlayerController::ChangePlayerControl);
	EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started, this, &AOBPlayerController::TogglePauseMenu);
	EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Started, this, &AOBPlayerController::Use);
	EnhancedInputComponent->BindAction(EndUseAction, ETriggerEvent::Completed, this, &AOBPlayerController::EndUse);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AOBPlayerController::Reload);
	EnhancedInputComponent->BindAction(SelectEquipmentAction, ETriggerEvent::Triggered, this, &AOBPlayerController::SelectEquipment);
	EnhancedInputComponent->BindAction(ToggleFireModeAction, ETriggerEvent::Started, this, &AOBPlayerController::ToggleFireMode);
}

void AOBPlayerController::FirstPersonMove(const FInputActionValue& Value)
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

void AOBPlayerController::FirstPersonLook(const FInputActionValue& Value)
{
	if (!ControlledCharacter) return;
	
	if (bMenuOpen) return;
	const FVector2D LookAxis = Value.Get<FVector2D>();
	ControlledCharacter->AddControllerYawInput(LookAxis.X);
	ControlledCharacter->AddControllerPitchInput(LookAxis.Y);
}

void AOBPlayerController::TopMove(const FInputActionValue& Value)
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

void AOBPlayerController::Jump()
{
	if (!ControlledCharacter || bMenuOpen) return;
	
	ControlledCharacter->Jump();
}

void AOBPlayerController::StopJumping()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->StopJumping();
}

void AOBPlayerController::Run()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AOBPlayerController::StopRun()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AOBPlayerController::Crouch()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
}

void AOBPlayerController::StopCrouch()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AOBPlayerController::ChangePlayerControl()
{
	if (!ControlledCharacter || bMenuOpen) return;
	
	ControlledCharacter->ChangePlayerControl();
}


void AOBPlayerController::TogglePauseMenu()
{
	bMenuOpen = !bMenuOpen;
	bShowMouseCursor = bMenuOpen;

	if (AOBHUD* HUD = Cast<AOBHUD>(GetHUD()))
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

void AOBPlayerController::Use()
{
	if (!ControlledCharacter) return;

	ControlledCharacter->HandleUse();
}

void AOBPlayerController::EndUse()
{
	if (!ControlledCharacter) return;
	
	ControlledCharacter->HandleEndUse();
}

void AOBPlayerController::Reload()
{
	if (!ControlledCharacter) return;

	ControlledCharacter->HandleReload();
}

void AOBPlayerController::SelectEquipment(const FInputActionValue& Value)
{
	const float SlotNumber = Value.Get<float>();
	const int32 SlotIndex = FMath::RoundToInt(SlotNumber);

	UE_LOG(LogTemp, Log, TEXT("[%s] Select Equipment Slot: %d"), CURRENT_CONTEXT, SlotIndex);
}

void AOBPlayerController::ToggleFireMode()
{
	if (!ControlledCharacter) return;

	ControlledCharacter->HandleToggleFireMode();
}
