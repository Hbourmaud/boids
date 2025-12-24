#include "BoidSpawner.h"
#include "Boid.h"
#include "Math/UnrealMathUtility.h"

ABoidSpawner::ABoidSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

void ABoidSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnBoids();
}

void ABoidSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAllBoids(DeltaTime);
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

void ABoidSpawner::UpdateAllBoids(float DeltaTime)
{
	const int32 NumBoids = SpawnedBoids.Num();

	if (NumBoids == 0) {
		return;
	}

	ParallelFor(NumBoids, [this](int32 Index)
		{
			ABoid* Boid = SpawnedBoids[Index];
			if (Boid && Boid->IsValidLowLevel()) {
				Boid->CalculateBoidBehaviors();
			}
		}
	);

	for (ABoid* Boid : SpawnedBoids) {
		if (Boid && Boid->IsValidLowLevel()) {
			Boid->ApplyMovement(DeltaTime);
		}
	}
}