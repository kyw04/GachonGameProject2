// Fill out your copyright notice in the Description page of Project Settings.


#include "AWeaponCollision.h"

// Sets default values
AAWeaponCollision::AAWeaponCollision()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	//RootComponent = WeaponMesh;
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

//void AAWeaponCollision::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
//{
//	//UE_LOG(LogTemp, Log, TEXT("NotifyHit"));
//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("Notify Actor Hit... Other Actor Name: %s"), *Other->GetName()));
//}

void AAWeaponCollision::NotifyActorBeginOverlap(AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("NotifyActorBeginOverlap"));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Notify Actor Begin Overlap... Other Actor Name: %s"), *OtherActor->GetName()));
}

void AAWeaponCollision::NotifyActorEndOverlap(AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("NotifyActorEndOverlap"));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Notify Actor End Overlap... Other Actor Name: %s"), *OtherActor->GetName()));
}