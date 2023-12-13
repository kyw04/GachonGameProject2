// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	Time = 0.0f;
	ShowTime = 0.0f;
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Time += DeltaTime;

	if (Player != nullptr)
	{
		if (!TakingAction && GetDistanceTo(Player) <= 150.0f)
		{
			TakingAction = true;
			Attack();
		}

		if (ShowTime <= Time)
		{
			Time -= ShowTime + 0.1f;
			GEngine->AddOnScreenDebugMessage(-1, 0.3f, FColor::Green, FString::Printf(TEXT("GetDistanceTo... Player Distance To Enmey: %f"), GetDistanceTo(Player)));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString::Printf(TEXT("Player is nullptr!!")));
	}
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemyCharacter::Attack()
{
	IsAttacking = true;
	int32 index = FMath::RandRange(0, AttackAnim.Num() - 1);
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Attack... Attack Index is: %d"), index));
	PlayAnimMontage(AttackAnim[index], 1, NAME_None);
}

void AEnemyCharacter::Block()
{
	IsBlocking = true;
	int32 index = FMath::RandRange(0, AttackAnim.Num() - 1);
	PlayAnimMontage(BlockAnim[index]);
}