// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GachonGameProject2Character.h"

#include "AWeaponCollision.generated.h"

UCLASS()
class GACHONGAMEPROJECT2_API AAWeaponCollision : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAWeaponCollision();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	//virtual void NotifyHit
	//(
	//	class UPrimitiveComponent* MyComp,
	//	AActor* Other,
	//	class UPrimitiveComponent* OtherComp,
	//	bool bSelfMoved,
	//	FVector HitLocation,
	//	FVector HitNormal,
	//	FVector NormalImpulse,
	//	const FHitResult& Hit
	//) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(BlueprintCallable)
	EState GetPlayerState() const;

	APlayerController* PlayerController;
	AGachonGameProject2Character* Player;

	UPROPERTY(EditAnywhere, Category = Mesh)
	UStaticMeshComponent* WeaponMesh;
};
