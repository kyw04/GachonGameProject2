// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TestActor.h"
#include "Spawner.generated.h"

UCLASS()
class GACHONGAMEPROJECT2_API ASpawner : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> ActorBluePrint;

	UPROPERTY(EditAnywhere)
	float SpawnDelay;
	float CurrentTime;
};
