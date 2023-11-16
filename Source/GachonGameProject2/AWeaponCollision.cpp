// Fill out your copyright notice in the Description page of Project Settings.


#include "AWeaponCollision.h"

// Sets default values
AAWeaponCollision::AAWeaponCollision()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

// Called when the game starts or when spawned
void AAWeaponCollision::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAWeaponCollision::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAWeaponCollision::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("NotifyHit"));
}

void AAWeaponCollision::NotifyActorBeginOverlap(AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("NotifyActorBeginOverlap"));
}