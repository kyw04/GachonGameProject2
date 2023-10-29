// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GachonGameProject2Character.generated.h"

UENUM(BlueprintType)
enum class EState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run"),
	Jump UMETA(DisplayName = "Jump"),
	Attack UMETA(DisplayName = "Attack"),
	AttackReady UMETA(DisplayName = "AttackReady"),
	Groggy UMETA(DisplayName = "Groggy"),
	WeaponChange UMETA(DisplayName = "WeaponChange")
};

UCLASS(config=Game)
class AGachonGameProject2Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackActon;

	
public:
	AGachonGameProject2Character();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EState State;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Info")
	float MaxHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Info")
	float Health = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Info")
	float MaxStamina = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	float StaminaPerSecond = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	float RecoveryStamina = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TArray<UAnimMontage*> AttackAnims;
	FVector2D AttackHand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* HoldAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DrawAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	float RestTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Info")
	float StartRecoveryStaminaTime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bOnSprint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SprintStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseWeapon;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void OnSprint();
	void EndSprint();

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	void Jump();

	void Attack();
	void ReadyAttack(const FInputActionValue& Value);
	void StaminaIsZero();
	void WeaponChange();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};

