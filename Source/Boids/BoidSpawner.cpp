#include "BoidSpawner.h"
#include "Boid.h"
#include "Math/UnrealMathUtility.h"

ABoidSpawner::ABoidSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

void ABoidSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnBoids();
}

void ABoidSpawner::SpawnBoids()
{
	SpawnedBoids.Empty();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector SpawnerLocation = GetActorLocation();

	for (int32 i = 0; i < NumberOfBoids; i++)
	{
		const FVector RandomOffset = FVector(
			FMath::RandRange(-SpawnRadius, SpawnRadius),
			FMath::RandRange(-SpawnRadius, SpawnRadius),
			FMath::RandRange(-SpawnRadius, SpawnRadius)
		);
		const FVector SpawnLocation = SpawnerLocation + RandomOffset;

		const FRotator RandomRotation = FRotator(
			FMath::RandRange(-180.0f, 180.0f),
			FMath::RandRange(-180.0f, 180.0f),
			FMath::RandRange(-180.0f, 180.0f)
		);

		ABoid* NewBoid = GetWorld()->SpawnActor<ABoid>(ABoid::StaticClass(), SpawnLocation, RandomRotation, SpawnParams);

		if (NewBoid)
		{
			NewBoid->Spawner = this;
			NewBoid->Speed = BoidSpeed;
			SpawnedBoids.Add(NewBoid);
		}
	}

	for (ABoid* Boid : SpawnedBoids)
	{
		if (Boid)
		{
			Boid->AllBoids = SpawnedBoids;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("BoidSpawner: Spawned %d boids"), SpawnedBoids.Num());
}