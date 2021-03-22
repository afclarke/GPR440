// Fill out your copyright notice in the Description page of Project Settings.


#include "InfluenceMap.h"
#include "DrawDebugHelpers.h"

AInfluenceMap::AInfluenceMap()
{
	PrimaryActorTick.bCanEverTick = true;
}

uint32 AInfluenceMap::GetGridIndexFromCoords(FVector2D coords) const
{
	CheckCoordsValid(coords);
	return coords.X + coords.Y * GridColumns;
}

FVector2D AInfluenceMap::GetGridCoordsFromWorldLoc(FVector loc) const
{
	FVector gridSpaceLoc = loc - mGridOrigin;
	FVector2D gridCoords = FVector2D(gridSpaceLoc.X / GridRows, gridSpaceLoc.Y / GridColumns);
	CheckCoordsValid(gridCoords);
	return gridCoords;
}

uint32 AInfluenceMap::GetGridIndexFromWorldLoc(FVector loc) const
{
	FVector2D gridCoords = GetGridCoordsFromWorldLoc(loc);
	return GetGridIndexFromCoords(gridCoords);
}

FVector AInfluenceMap::GetWorldLocFromCoords(FVector2D coords) const
{
	CheckCoordsValid(coords);
	return FVector(
		mGridOrigin.X + mCellHalfDims.X + mCellDims.X * coords.X,
		mGridOrigin.Y + mCellHalfDims.Y + mCellDims.Y * coords.Y,
		mGridOrigin.Z);
}

void AInfluenceMap::CheckCoordsValid(FVector2D coords) const
{
	check(coords.X < 0 || coords.X >= GridRows || coords.Y < 0 || coords.Y >= GridColumns)
}

uint8 AInfluenceMap::GetValueAtIndex(uint32 index) const
{
	return mValues[index];
}

uint8 AInfluenceMap::GetValueAtCoords(FVector2D coords) const
{
	CheckCoordsValid(coords);
	uint32 index = GetGridIndexFromCoords(coords);
	return GetValueAtIndex(index);
}

uint32 AInfluenceMap::GetHighestCellIndex() const
{
	return mHighestCellIndex;
}

void AInfluenceMap::CheckMapsCompatible(const AInfluenceMap& mapA, const AInfluenceMap& mapB)
{
	check(mapA.GridRows == mapB.GridRows && mapA.GridColumns == mapB.GridColumns);
}

void AInfluenceMap::CheckHighestCell(uint32 i, uint8& highestValue)
{
	// check for highest value cell
	if (mValues[i] >= highestValue)
	{
		highestValue = mValues[i];
		mHighestCellIndex = i;
	}
}

void AInfluenceMap::AddMap(const AInfluenceMap& map, float scalar, bool updateHighestPoint)
{
	CheckMapsCompatible(*this, map);
	uint8 highestValue = -1;

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] += map.mValues[i] * scalar;

		if(updateHighestPoint)
		{
			CheckHighestCell(i, highestValue);
		}
	}
}

void AInfluenceMap::MultiplyMap(const AInfluenceMap& map, float scalar, bool updateHighestPoint)
{
	CheckMapsCompatible(*this, map);
	uint8 highestValue = -1;

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] *= map.mValues[i] * scalar;

		if (updateHighestPoint)
		{
			CheckHighestCell(i, highestValue);
		}
	}
}

void AInfluenceMap::ScaleMap(float scalar, bool updateHighestPoint)
{
	uint8 highestValue = -1;

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] *= scalar;

		// TODO: This is only necessary if map values & scalars can be negative
		if (updateHighestPoint)
		{
			CheckHighestCell(i, highestValue);
		}
	}
}

void AInfluenceMap::InvertMap(bool updateHighestPoint)
{
	uint8 highestValue = -1;

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] = 1 - mValues[i];

		if (updateHighestPoint)
		{
			CheckHighestCell(i, highestValue);
		}
	}
}

void AInfluenceMap::DrawGrid() const
{
	UWorld* pWorld = GetWorld();
	for (uint32 i = 0; i < GridRows; i++)
	{
		for (uint32 j = 0; j < GridColumns; j++)
		{
			FVector2D coords = FVector2D(i, j);
			FVector cellCenter = GetWorldLocFromCoords(coords);
			uint32 cellIndex = GetGridIndexFromCoords(coords);
			uint8 cellValue = mValues[cellIndex];

			DrawDebugBox(pWorld, cellCenter, mCellHalfDims, FColor::Yellow,
				false, -1, 0, 20);
			cellCenter.Z += 1;
			FColor valueColor = FColor(DebugValueColor.R, DebugValueColor.G, DebugValueColor.B, cellValue);
			DrawDebugSolidBox(pWorld, FBox(cellCenter - mCellHalfDims, cellCenter + mCellHalfDims),
				valueColor);
		}
	}
}

void AInfluenceMap::BeginPlay()
{
	Super::BeginPlay();

	mGridOrigin = GetActorLocation();
	mCellDims = FVector(GridWidth / GridRows, GridHeight / GridColumns, 0);
	mCellHalfDims = mCellDims / 2.0f;
	mValues.SetNumZeroed(GridRows * GridColumns);
}

void AInfluenceMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DebugDrawGrid)
	{
		DrawGrid();
	}
}

