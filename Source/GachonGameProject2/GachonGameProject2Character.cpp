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
		RestTime += DeltaTime * 0.7f;
		if (RestTime >= StartRecoveryStaminaTime)
		{
			StopAnimMontage(GroggyAnim);
			SmoothLegsValue = 1.0f;
			State = EState::Idle;
		}
		else
		{
			SmoothLegsValue = 0.0f;
			PlayAnimMontage(GroggyAnim, 1.0f, NAME_None);
		}
	}
	else
	{
		RestTime += DeltaTime;
	}

	switch (State)
	{
	case EState::Groggy:
	case EState::Idle:
		if (Stamina <= 0.0f)
			State = EState::Groggy;
		break;
	case EState::Walk:
	case EState::Run:
	case EState::Jump:
	case EState::Attack:
	case EState::AttackReady:
	case EState::WeaponChange:
	case EState::Roll:
	case EState::Block:
	case EState::Blocking:
		RestTime = 0.0f;
		break;
	default:
		break;
	}

	if (BlockStart != 0)
		BlockStart += DeltaTime;

	PublicDeltaTime = DeltaTime;
	RecoveryStamina = StaminaPerSecond * DeltaTime;
	SprintStamina = 1.0f * DeltaTime;
}

bool AGachonGameProject2Character::ReduceStamina(float Value)
{
	RestTime = 0.0f;
	Stamina -= Value;
	if (Stamina <= 0.0f)
	{
		StaminaIsZero();
		return true;
	}

	return false;
}

void AGachonGameProject2Character::StaminaIsZero()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("Groggy")));

	Stamina = 0.0f;
	State = EState::Groggy;
}

void AGachonGameProject2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	bUseWeapon = false;
	BlockStart = 0;

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
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AGachonGameProject2Character::OnBlock);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AGachonGameProject2Character::EndBlock);
	}
}

void AGachonGameProject2Character::Jump()
{
	switch (State)
	{
	case EState::Attack: case EState::Groggy: case EState::WeaponChange: 
	case EState::Roll: case EState::Block:
		return;
	}

	if (ACharacter::JumpKeyHoldTime == 0 && ACharacter::CanJump())
	{
		if (ReduceStamina(5.0f))
			return;
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
			State = EState::Run;

			if (bUseWeapon)
			{
				WeaponChange();
				return;
			}

			//UE_LOG(LogTemp, Log, TEXT("sprint"));
			if (ReduceStamina(SprintStamina))
				return;

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

	ReduceStamina(25.0f);
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

	// x + y = { -1 = left, 0 = mid, 1 = right }
	AttackHand = Value.Get<FVector2D>();
	UseHandIndex = AttackHand.X + AttackHand.Y + 1;

	if (bOnBlock)
	{
		PlayBlock();
		return;
	}

	if (AttackHoldTime == 0) // rotate in the direction the camera is looking
	{
		bAttackHoldOn = false;
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		SetActorRotation(YawRotation);
	}
	AttackHoldTime += PublicDeltaTime;

	if (AttackAnims[UseHandIndex])
	{
		PlayAnimMontage(AttackAnims[UseHandIndex], 1, NAME_None);
	}

	if (AttackHoldTime >= 0.6f && !bAttackHoldOn)
	{
		bAttackHoldOn = true;
		ReduceStamina(HoldAttackStamina);
		Gameplay->SpawnEmitterAttached(AttackHoldParticleAsset, RootComponent, FName("Sword_End"));
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
	if (BlockStart != 0)
	{
		StopBlock();
	}

	if (!ACharacter::CanJump() || State != EState::AttackReady)
		return;

	State = EState::Attack;
	AttackHoldTime = 0;

	if (!bAttackHoldOn)
		ReduceStamina(AttackStamina);

	if (AttackAnims[UseHandIndex])
	{
		PlayAnimMontage(AttackAnims[UseHandIndex], Proficiency(UseHandIndex), NAME_None);
	}

	AttackHand = AttackHand.Zero();
}

void AGachonGameProject2Character::OnBlock()
{
	bOnBlock = true;
}
void AGachonGameProject2Character::EndBlock()
{
	StopBlock();
	bBlockSuccess = false;
	bOnBlock = false;
}

void AGachonGameProject2Character::PlayBlock()
{
	State = EState::Block;
	AttackHoldTime = 0;

	if (BlockStart == 0 && BlockAnims[UseHandIndex])
	{
		UE_LOG(LogTemp, Log, TEXT("%d"), UseHandIndex);
		Proficiency(UseHandIndex);
		BlockStart = 1.0f;
		SmoothLegsValue = 0.0f;
		AttackHand = AttackHand.Zero();
		PlayAnimMontage(BlockAnims[UseHandIndex], 1, NAME_None);
	}
}

void AGachonGameProject2Character::StopBlock()
{
	if (State != EState::Block)
		return;

	for (int i = 0; i < 3; i++)
	{
		StopAnimMontage(BlockAnims[i]);
	}

	if (bBlockSuccess)
	{
		ReduceStamina(BlockFailureStamina);
	}
	else
	{
		ReduceStamina(BlockSuccessStamina);
	}

	SmoothLegsValue = 1.0f;
	State = EState::Idle;
	BlockStart = 0;
}

void AGachonGameProject2Character::Blocking(AActor* OtherActor)
{
	bBlockSuccess = true;
	if (BlockStart <= 1.25f)
	{
		Gameplay->SpawnEmitterAttached(ParryingParticleAsset, GetMesh(), FName(TEXT("Blade_Center")));

		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Silver, FString::Printf(TEXT("Parrying")));
	}
	else
	{
		Gameplay->SpawnEmitterAttached(BlockParticleAsset, GetMesh(), FName(TEXT("Blade_Center")));

		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("Block")));
	}
}

float AGachonGameProject2Character::Proficiency(int index)
{
	if (index == 0)
	{
		LeftHandUseCount++;
		RightHandUseCount--;
	}
	if (index == 2)
	{
		LeftHandUseCount--;
		RightHandUseCount++;
	}

	if (LeftHandUseCount < 1)
		LeftHandUseCount = 1;
	if (LeftHandUseCount > MaxUseCount)
		LeftHandUseCount = MaxUseCount;
	
	if (RightHandUseCount < 1)
		RightHandUseCount = 1;
	if (RightHandUseCount > MaxUseCount)
		RightHandUseCount = MaxUseCount;


	LeftProficiency = LeftHandUseCount / MaxUseCount;
	RightProficiency = RightHandUseCount / MaxUseCount;

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("Proficiency... Index : %d"), index));

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("Proficiency... Left Hand Proficiency : %f"), LeftProficiency));
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("Proficiency... Right Hand Proficiency : %f"), RightProficiency));
	
	if (index == 0)
		return LeftProficiency;
	if (index == 1)
		return 1;
	return RightProficiency;
}
