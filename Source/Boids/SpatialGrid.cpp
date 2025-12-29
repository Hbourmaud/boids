#include "SpatialGrid.h"
#include "Boid.h"

FSpatialGrid::FSpatialGrid(const FVector& InOrigin, float InCellSize, int32 InGridSizeX, int32 InGridSizeY, int32 InGridSizeZ)
	: Origin(InOrigin), CellSize(InCellSize), GridSizeX(InGridSizeX), GridSizeY(InGridSizeY), GridSizeZ(InGridSizeZ) {
}

void FSpatialGrid::Clear() {
	Grid.Reset();
}

void FSpatialGrid::AddBoid(ABoid* Boid, const FVector& Position) {
	if (!Boid) {
		return;
	}

	const FIntVector CellIndex = GetCellIndex(Position);
	if (IsValidCell(CellIndex)) {
		const int32 Hash = CellToHash(CellIndex);
		Grid.FindOrAdd(Hash).Add(Boid);
	}
}

TArray<ABoid*> FSpatialGrid::GetNearbyBoids(const FVector& Position, float Radius) const {
	TArray<ABoid*> NearbyBoids;

	const FIntVector CenterCell = GetCellIndex(Position);
	const int32 CellRadius = FMath::CeilToInt(Radius / CellSize);


	// todo : refacto
	for (int32 x = -CellRadius; x <= CellRadius; x++) {
		for (int32 y = -CellRadius; y <= CellRadius; y++) {
			for (int32 z = -CellRadius; z <= CellRadius; z++) {
				const FIntVector CheckCell = CenterCell + FIntVector(x, y, z);

				if (IsValidCell(CheckCell)) {
					const int32 Hash = CellToHash(CheckCell);
					const TArray<ABoid*>* CellBoids = Grid.Find(Hash);

					if (CellBoids) {
						NearbyBoids.Append(*CellBoids);
					}
				}
			}
		}
	}

	return NearbyBoids;
}

FIntVector FSpatialGrid::GetCellIndex(const FVector& Position) const {
	const FVector LocalPos = Position - Origin;
	return FIntVector(
		FMath::FloorToInt(LocalPos.X / CellSize),
		FMath::FloorToInt(LocalPos.Y / CellSize),
		FMath::FloorToInt(LocalPos.Z / CellSize)
		);
}

int32 FSpatialGrid::CellToHash(const FIntVector& CellIndex) const {
	// todo comment
	return CellIndex.X + CellIndex.Y * GridSizeX + CellIndex.Z * GridSizeX * GridSizeY;
}

bool FSpatialGrid::IsValidCell(const FIntVector& CellIndex) const {
	return CellIndex.X >= 0 && CellIndex.X < GridSizeX &&
		CellIndex.Y >= 0 && CellIndex.Y < GridSizeY &&
		CellIndex.Z >= 0 && CellIndex.Z < GridSizeZ;
}