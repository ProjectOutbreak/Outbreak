// Fill out your copyright notice in the Description page of Project Settings.

#include "OBPlayerController.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"

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
}

void AOBPlayerController::FirstPersonMove(const FInputActionValue& Value)
{
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
	if (!ControlledCharacter) return;
	
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
	if (!ControlledCharacter) return;
	
	ControlledCharacter->ChangePlayerControl();
}
