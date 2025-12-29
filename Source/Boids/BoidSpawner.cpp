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

	InitializeSpatialGrid();
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

void ABoidSpawner::InitializeSpatialGrid() {
	const FVector SpawnerLocation = GetActorLocation();
	const FVector GridOrigin = SpawnerLocation - FVector(WorldExtent * 0.5f);

	const int32 GridSizeX = FMath::CeilToInt(WorldExtent / GridCellSize);
	const int32 GridSizeY = FMath::CeilToInt(WorldExtent / GridCellSize);
	const int32 GridSizeZ = FMath::CeilToInt(WorldExtent / GridCellSize);

	SpatialGrid = MakeUnique<FSpatialGrid>(GridOrigin, GridCellSize, GridSizeX, GridSizeY, GridSizeZ);
}

void ABoidSpawner::RebuildSpatialGrid() {
	if (!SpatialGrid.IsValid()) {
		return;
	}

	SpatialGrid->Clear();

	for (ABoid* Boid : SpawnedBoids) {
		if (Boid && Boid->IsValidLowLevel()) {
			SpatialGrid->AddBoid(Boid, Boid->GetActorLocation());
		}
	}

	if (ShowDebugGrid) {
		const FVector& Origin = SpatialGrid->Origin;
		const float CellSize = SpatialGrid->CellSize;
		const int32 GridSizeX = SpatialGrid->GridSizeX;
		const int32 GridSizeY = SpatialGrid->GridSizeY;
		const int32 GridSizeZ = SpatialGrid->GridSizeZ;

		const FVector GridExtent = FVector(GridSizeX * CellSize, GridSizeY * CellSize, GridSizeZ * CellSize);
		DrawDebugBox(GetWorld(), Origin + GridExtent * 0.5f, GridExtent * 0.5f, FColor::Yellow, false, -1.0f, 0, 5.0f);

		const int32 ZLevel = GridSizeZ / 2;
		for (int32 x = 0; x <= GridSizeX; x++)
		{
			const FVector Start = Origin + FVector(x * CellSize, 0, ZLevel * CellSize);
			const FVector End = Origin + FVector(x * CellSize, GridSizeY * CellSize, ZLevel * CellSize);
			DrawDebugLine(GetWorld(), Start, End, FColor::White, false, -1.0f, 0, 1.0f);
		}
		for (int32 y = 0; y <= GridSizeY; y++)
		{
			const FVector Start = Origin + FVector(0, y * CellSize, ZLevel * CellSize);
			const FVector End = Origin + FVector(GridSizeX * CellSize, y * CellSize, ZLevel * CellSize);
			DrawDebugLine(GetWorld(), Start, End, FColor::White, false, -1.0f, 0, 1.0f);
		}

		for (ABoid* Boid : SpawnedBoids)
		{
			if (Boid)
			{
				const FVector BoidPos = Boid->GetActorLocation();
				const FIntVector CellIndex = SpatialGrid->GetCellIndex(BoidPos);

				const FVector CellMin = Origin + FVector(CellIndex.X * CellSize, CellIndex.Y * CellSize, CellIndex.Z * CellSize);
				const FVector CellCenter = CellMin + FVector(CellSize * 0.5f);
				DrawDebugBox(GetWorld(), CellCenter, FVector(CellSize * 0.5f), FColor::Green, false, -1.0f, 0, 2.0f);
			}
		}
	}
}

void ABoidSpawner::UpdateAllBoids(float DeltaTime)
{
	const int32 NumBoids = SpawnedBoids.Num();

	if (NumBoids == 0) {
		return;
	}

	RebuildSpatialGrid();

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