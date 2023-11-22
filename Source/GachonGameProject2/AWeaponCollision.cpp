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
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PlayerController)
		Player = Cast<AGachonGameProject2Character>(PlayerController->GetPawn());

	if (!Player)
		UE_LOG(LogTemp, Error, TEXT("Player Not Found"));
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
	//UE_LOG(LogTemp, Log, TEXT("NotifyActorBeginOverlap"));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Notify Actor Begin Overlap... Other Actor Name: %s"), *OtherActor->GetName()));
	switch (GetPlayerState())
	{
	case EState::Attack:
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Attack")));
		break;
	case EState::Block:
		Player->Block();
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("State Conditions Not Met")));
	}
}

void AAWeaponCollision::NotifyActorEndOverlap(AActor* OtherActor)
{
	//UE_LOG(LogTemp, Log, TEXT("NotifyActorEndOverlap"));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Notify Actor End Overlap... Other Actor Name: %s"), *OtherActor->GetName()));
}

EState AAWeaponCollision::GetPlayerState() const
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::Printf(TEXT("Get Player State... Current Player State: %s"), *UEnum::GetValueAsString(Player->State)));
	//UE_LOG(LogTemp, Log, TEXT("Player State Enum: %s"), *UEnum::GetValueAsString(Player->State));
	return Player->State;
}