// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TestActor.generated.h"

UCLASS()
class GACHONGAMEPROJECT2_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float Speed;
	UPROPERTY(EditAnywhere)
	float MovementTime;
	float CurrentTime;

	UPROPERTY(EditAnywhere)
	FVector Direction;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
