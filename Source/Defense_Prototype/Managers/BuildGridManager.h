// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildGridManager.generated.h"

class AWallActor;

UCLASS()
class ABuildGridManager : public AActor
{
	GENERATED_BODY()

public:
	ABuildGridManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridSizeX = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridSizeY = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float CellSize = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Debug")
	bool bDrawGrid = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Debug")
	float DebugZOffset = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Debug")
	FColor GridLineColor = FColor(0, 255, 255, 255);

	UFUNCTION(BlueprintCallable)
	FIntPoint WorldToGrid(const FVector& WorldLocation) const;

	UFUNCTION(BlueprintCallable)
	FVector GridToWorld(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintCallable)
	bool CanPlaceAt(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintCallable)
	AWallActor* PlaceWall(const FIntPoint& Coord, TSubclassOf<AWallActor> WallClass);

protected:
	UPROPERTY()
	TSet<FIntPoint> Occupied;

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	void DrawGrid() const;
};


