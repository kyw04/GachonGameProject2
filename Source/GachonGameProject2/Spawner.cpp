// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"
#include "Misc/DateTime.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentTime = 0.0f;

	//static ConstructorHelpers::FObjectFinder<Blueprint> AmmoItem(TEXT("Blueprint'/Game/C++ Classes/GachonGameProject2/TestActor.TestActor'"));
	//if (AmmoItem.Object)
	//{
	//	Actor = (UClass*)AmmoItem.Object;
	//}
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CurrentTime += DeltaTime;
	if (CurrentTime >= SpawnDelay)
	{
		CurrentTime -= SpawnDelay; 
		UWorld* world = GetWorld();
		if (world)
		{
			UE_LOG(LogTemp, Log, TEXT("SpawnActor"));

			int32 Random = FMath::RandRange(1, 10);
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			FRotator rotator;
			FVector  SpawnLocation = GetActorLocation();
			SpawnLocation.Z -= 90.0f;

			world->SpawnActor<AActor>(ActorBluePrint, SpawnLocation, rotator, SpawnParams);
		}
	}
}

// Called to bind functionality to input
void ASpawner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

