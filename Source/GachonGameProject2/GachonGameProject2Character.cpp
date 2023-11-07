// Copyright Epic Games, Inc. All Rights Reserved.

#include "GachonGameProject2Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AGachonGameProject2Character

AGachonGameProject2Character::AGachonGameProject2Character()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AGachonGameProject2Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (State == EState::Groggy)
	{
		RestTime += DeltaTime * 2.5f;
		if (RestTime >= StartRecoveryStaminaTime)
		{
			State = EState::Idle;
		}
	}
	else
	{
		RestTime += DeltaTime * 5.0f;
	}

	PublicDeltaTime = DeltaTime;
	RecoveryStamina = StaminaPerSecond * DeltaTime;
	SprintStamina = 1.0f * DeltaTime;
}

void AGachonGameProject2Character::StaminaIsZero()
{
	Stamina = 0.0f;
	State = EState::Groggy;
}

void AGachonGameProject2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	bUseWeapon = false;

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGachonGameProject2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGachonGameProject2Character::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGachonGameProject2Character::Move);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AGachonGameProject2Character::OnSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AGachonGameProject2Character::EndSprint);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGachonGameProject2Character::Look);

		//Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AGachonGameProject2Character::ReadyAttack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AGachonGameProject2Character::Attack);

		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AGachonGameProject2Character::Roll);
	}

}

void AGachonGameProject2Character::Jump()
{
	if (Stamina < 5.0f)
		return;

	switch (State)
	{
	case EState::Attack:
	case EState::Groggy:
	case EState::WeaponChange:
	case EState::Roll:
		return;
	}

	RestTime = 0.0f;
	if (ACharacter::JumpKeyHoldTime == 0 && ACharacter::CanJump())
	{
		Stamina -= 5.0f;
		if (Stamina < 0.0f)
		{
			StaminaIsZero();
			return;
		}
	}

	ACharacter::Jump();
}

void AGachonGameProject2Character::Move(const FInputActionValue& Value)
{
	if (State != EState::Idle)
		return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	MovementVector = MovementVector.GetSafeNormal();

	if (Controller != nullptr)
	{
		float speed = 0.5f;
		State = EState::Walk;
		//UE_LOG(LogTemp, Log, TEXT("%s"), bOnSprint ? TEXT("true") : TEXT("flse"));
		if (bOnSprint)
		{
			RestTime = 0.0f;
			State = EState::Run;

			if (bUseWeapon)
			{
				WeaponChange();
				return;
			}

			//UE_LOG(LogTemp, Log, TEXT("sprint"));
			Stamina -= SprintStamina;
			if (Stamina <= 0.0f)
			{
				StaminaIsZero();
				return;
			}

			speed = 1.0f;
		}

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * speed;

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * speed;

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

	State = EState::Idle;
}
void AGachonGameProject2Character::OnSprint()
{
	bOnSprint = true;
	//UE_LOG(LogTemp, Log, TEXT("OnSprint"));
}
void AGachonGameProject2Character::EndSprint()
{
	bOnSprint = false;
	//UE_LOG(LogTemp, Log, TEXT("EndSprint"));
}


void AGachonGameProject2Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Log, TEXT("%f %f"), LookAxisVector.X, LookAxisVector.Y);

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGachonGameProject2Character::Roll()
{
	if (State != EState::Idle || !ACharacter::CanJump())
		return;

	State = EState::Roll;
	PlayAnimMontage(RollAnim, 1, NAME_None);
}

void AGachonGameProject2Character::WeaponChange()
{
	State = EState::WeaponChange;
	bUseWeapon = !bUseWeapon;

	if (bUseWeapon)
		PlayAnimMontage(DrawAnim, 1, NAME_None);
	else
		PlayAnimMontage(HoldAnim, 1, NAME_None);
}

void AGachonGameProject2Character::ReadyAttack(const FInputActionValue& Value)
{
	if (!ACharacter::CanJump())
		return;

	switch (State)
	{
	case EState::Attack:
	case EState::Groggy:
	case EState::WeaponChange:
		return;
	}

	if (!bUseWeapon)
	{
		WeaponChange();
		return;
	}

	if (AttackHoldTime == 0) // rotate in the direction the camera is looking
	{
		bAttackHoldOn = false;
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		SetActorRotation(YawRotation);
	}

	// x + y = { -1 = left, 0 = mid, 1 = right }
	AttackHand = Value.Get<FVector2D>();
	AttackHoldTime += PublicDeltaTime;

	int index = AttackHand.X + AttackHand.Y + 1;
	if (AttackAnims[index])
	{
		PlayAnimMontage(AttackAnims[index], 1, NAME_None);
	}

	if (AttackHoldTime >= 0.6f && !bAttackHoldOn)
	{
		bAttackHoldOn = true;
		Gameplay->SpawnEmitterAttached(ParticleAsset, RootComponent, FName("Sword_End"));
	}
	if (AttackHoldTime >= 1.25f)
	{
		Attack();
		return;
	}

	State = EState::AttackReady;
	//UE_LOG(LogTemp, Log, TEXT("%f\nTime: %f"), AttackHand.X + AttackHand.Y, AttackHoldTime);
}

void AGachonGameProject2Character::Attack()
{
	if (!ACharacter::CanJump() || State != EState::AttackReady)
		return;
	State = EState::Attack;
	AttackHoldTime = 0;

	int index = AttackHand.X + AttackHand.Y + 1;
	if (AttackAnims[index])
	{
		PlayAnimMontage(AttackAnims[index], 1, NAME_None);
	}

	switch (index)
	{
	case 0:
		UE_LOG(LogTemp, Log, TEXT("left"));
		break;
	case 1:
		UE_LOG(LogTemp, Log, TEXT("middle"));
		break;
	case 2:
		UE_LOG(LogTemp, Log, TEXT("right"));
		break;
	}

	AttackHand = AttackHand.Zero();
}

