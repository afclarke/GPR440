// Fill out your copyright notice in the Description page of Project Settings.


#include "InfluenceMap.h"
#include "DrawDebugHelpers.h"

AInfluenceMap::AInfluenceMap()
{
	PrimaryActorTick.bCanEverTick = true;
}

uint32 AInfluenceMap::GetGridIndexFromCoords(FVector2D coords) const
{
	return coords.X + coords.Y * GridColumns;
}

FVector2D AInfluenceMap::GetGridCoordsFromWorldLoc(FVector loc) const
{
	FVector gridSpaceLoc = loc - mGridOrigin;
	return FVector2D(gridSpaceLoc.X / GridRows, gridSpaceLoc.Y / GridColumns);
}

uint32 AInfluenceMap::GetGridIndexFromWorldLoc(FVector loc) const
{
	FVector2D gridCoords = GetGridCoordsFromWorldLoc(loc);
	return GetGridIndexFromCoords(gridCoords);
}

FVector AInfluenceMap::GetWorldLocFromCoords(FVector2D coords) const
{
	return FVector(
		mGridOrigin.X + mCellHalfDims.X + mCellDims.X * coords.X,
		mGridOrigin.Y + mCellHalfDims.Y + mCellDims.Y * coords.Y,
		mGridOrigin.Z);
}

void AInfluenceMap::DrawGrid() const
{
	UWorld* pWorld = GetWorld();
	for (uint32 i = 0; i < GridRows; i++)
	{
		for (uint32 j = 0; j < GridColumns; j++)
		{
			FVector cellCenter = GetWorldLocFromCoords(FVector2D(i, j));

			DrawDebugBox(pWorld, cellCenter, mCellHalfDims, FColor::Yellow,
				false, -1, 0, 20);
		}
	}
}

void AInfluenceMap::BeginPlay()
{
	Super::BeginPlay();

	mGridOrigin = GetActorLocation();
	mCellDims = FVector(GridWidth / GridRows, GridHeight / GridColumns, 0);
	mCellHalfDims = mCellDims / 2.0f;
}

void AInfluenceMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DebugDrawGrid)
	{
		DrawGrid();
	}
}

