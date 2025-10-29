// Copyright Epic Games, Inc. All Rights Reserved.

#include "Managers/BuildGridManager.h"
#include "Actors/WallActor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

ABuildGridManager::ABuildGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

FIntPoint ABuildGridManager::WorldToGrid(const FVector& WorldLocation) const
{
	const FVector Local = WorldLocation - GetActorLocation();
	const int32 X = FMath::RoundToInt(Local.X / CellSize);
	const int32 Y = FMath::RoundToInt(Local.Y / CellSize);
	return FIntPoint(X, Y);
}

FVector ABuildGridManager::GridToWorld(const FIntPoint& Coord) const
{
	return GetActorLocation() + FVector(Coord.X * CellSize, Coord.Y * CellSize, 0.f);
}

bool ABuildGridManager::CanPlaceAt(const FIntPoint& Coord) const
{
	if (Coord.X < -GridSizeX || Coord.X > GridSizeX || Coord.Y < -GridSizeY || Coord.Y > GridSizeY)
	{
		return false;
	}
	return !Occupied.Contains(Coord);
}

AWallActor* ABuildGridManager::PlaceWall(const FIntPoint& Coord, TSubclassOf<AWallActor> WallClass)
{
	if (!CanPlaceAt(Coord) || !*WallClass)
	{
		return nullptr;
	}
	const FVector SpawnLoc = GridToWorld(Coord);
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AWallActor* Wall = GetWorld()->SpawnActor<AWallActor>(WallClass, SpawnLoc, FRotator::ZeroRotator, Params);
	if (Wall)
	{
		Occupied.Add(Coord);
	}
	return Wall;
}

void ABuildGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (bDrawGrid)
	{
		DrawGrid();
	}
}

void ABuildGridManager::DrawGrid() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	const FVector Origin = GetActorLocation() + FVector(0.f, 0.f, DebugZOffset);
	const int32 MinX = -GridSizeX;
	const int32 MaxX = GridSizeX;
	const int32 MinY = -GridSizeY;
	const int32 MaxY = GridSizeY;

	// Draw vertical lines (varying Y)
	for (int32 X = MinX; X <= MaxX; ++X)
	{
		const FVector A = Origin + FVector(X * CellSize, MinY * CellSize, 0.f);
		const FVector B = Origin + FVector(X * CellSize, MaxY * CellSize, 0.f);
		DrawDebugLine(World, A, B, GridLineColor, false, 0.f, 0, 1.f);
	}
	// Draw horizontal lines (varying X)
	for (int32 Y = MinY; Y <= MaxY; ++Y)
	{
		const FVector A = Origin + FVector(MinX * CellSize, Y * CellSize, 0.f);
		const FVector B = Origin + FVector(MaxX * CellSize, Y * CellSize, 0.f);
		DrawDebugLine(World, A, B, GridLineColor, false, 0.f, 0, 1.f);
	}
}


