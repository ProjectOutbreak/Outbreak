// Fill out your copyright notice in the Description page of Project Settings.

#include "OBPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Outbreak/UI/OBHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Game/Interface/InteractInterface.h"
#include "Outbreak/UI/OBWidget.h"
#include "DrawDebugHelpers.h"


AOBPlayerController::AOBPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionDistance = 250.0f;
	
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
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionChangeCamRef(TEXT("/Game/Inputs/Actions/IA_ChangeControl.IA_ChangeControl"));
	if (InputActionChangeCamRef.Object)
	{
		ChangePlayerControlAction = InputActionChangeCamRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionToggleMenuRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_ToggleMenu.IA_ToggleMenu'"));
	if (InputActionToggleMenuRef.Object)
	{
		ToggleMenuAction = InputActionToggleMenuRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionInteractRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Inputs/IA_DoorInteract.IA_DoorInteract'"));
	if (InputActionInteractRef.Object)
	{
		InteractAction = InputActionInteractRef.Object;
	}
}

void AOBPlayerController::Tick(float DeltaTime)
{
	GetInteractableObject();
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
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AOBPlayerController::PerformInteract);
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

void AOBPlayerController::PerformInteract()
{
	if (FocusedInteractable)
	{
		FocusedInteractable->Execute_Interact(FocusedInteractable.GetObject(), GetPawn());
	}
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

void AOBPlayerController::GetInteractableObject()
{
	
	FVector CameraLocation;
	FRotator CameraRotation;
	GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * InteractionDistance);
    
	FHitResult HitResult;
	FCollisionQueryParams Params;
	if (GetPawn())
	{
		Params.AddIgnoredActor(GetPawn());
	}

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, TraceEnd, ECC_Visibility, Params);

	TScriptInterface<IInteractInterface> CurrentHitObject = bHit ? HitResult.GetActor() : nullptr;

	if (CurrentHitObject != FocusedInteractable)
	{
		if (FocusedInteractable)
		{
			FocusedInteractable->Execute_EndFocus(FocusedInteractable.GetObject());
		}

		if (CurrentHitObject)
		{
			CurrentHitObject->Execute_BeginFocus(CurrentHitObject.GetObject());
		}

		FocusedInteractable = CurrentHitObject;
	}
}
