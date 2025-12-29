#pragma once

#include "CoreMinimal.h"

class ABoid;

class FSpatialGrid
{
public:
	FSpatialGrid(const FVector& InOrigin, float InCellSize, int32 InGridSizeX, int32 InGridSizeY, int32 InGridSizeZ);

	void Clear();

	void AddBoid(ABoid* Boid, const FVector& Position);

	TArray<ABoid*> GetNearbyBoids(const FVector& Position, float Radius) const;

	FIntVector GetCellIndex(const FVector& Position) const;

	FVector Origin;
	float CellSize;
	int32 GridSizeX;
	int32 GridSizeY;
	int32 GridSizeZ;

	TMap<int32, TArray<ABoid*>> Grid;

	int32 CellToHash(const FIntVector& CellIndex) const;

	bool IsValidCell(const FIntVector& CellIndex) const;
};