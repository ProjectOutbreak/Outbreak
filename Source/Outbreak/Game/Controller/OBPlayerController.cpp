// Fill out your copyright notice in the Description page of Project Settings.

#include "OBPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Outbreak/UI/OBHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/UI/OBWidget.h"

AOBPlayerController::AOBPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Move.IA_Move'"));
	if (InputActionMoveRef.Object)
	{
		MoveAction = InputActionMoveRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Look.IA_Look'"));
	if (InputActionLookRef.Object)
	{
		LookAction = InputActionLookRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Jump.IA_Jump'"));
	if (InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSprintRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Sprint.IA_Sprint'"));
	if (InputActionSprintRef.Object)
	{
		SprintAction = InputActionSprintRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionCrouchRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_Crouch.IA_Crouch'"));
	if (InputActionCrouchRef.Object)
	{
		CrouchAction = InputActionCrouchRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionToggleMenuRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_ToggleMenu.IA_ToggleMenu'"));
	if (InputActionToggleMenuRef.Object)
	{
		ToggleMenuAction = InputActionToggleMenuRef.Object;
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
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOBPlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOBPlayerController::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AOBPlayerController::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AOBPlayerController::StopJumping);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AOBPlayerController::Run);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AOBPlayerController::StopRun);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AOBPlayerController::Crouch);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AOBPlayerController::StopCrouch);
	EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started, this, &AOBPlayerController::TogglePauseMenu);
}

void AOBPlayerController::Move(const FInputActionValue& Value)
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

void AOBPlayerController::Look(const FInputActionValue& Value)
{
	if (bMenuOpen) return;
	const FVector2D LookAxis = Value.Get<FVector2D>();
	if (ControlledCharacter)
	{
		ControlledCharacter->AddControllerYawInput(LookAxis.X);
		ControlledCharacter->AddControllerPitchInput(LookAxis.Y);
	}
}

void AOBPlayerController::Jump()
{
	if (bMenuOpen) return;
	if (ControlledCharacter)
	{
		ControlledCharacter->Jump();
	}
}

void AOBPlayerController::StopJumping()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->StopJumping();
	}
}

void AOBPlayerController::Run()
{
	if (bMenuOpen) return;
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AOBPlayerController::StopRun()
{
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AOBPlayerController::Crouch()
{
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
}

void AOBPlayerController::StopCrouch()
{
	ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
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
