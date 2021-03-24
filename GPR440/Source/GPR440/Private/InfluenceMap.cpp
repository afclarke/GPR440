// Fill out your copyright notice in the Description page of Project Settings.


#include "InfluenceMap.h"
#include "DrawDebugHelpers.h"

PRAGMA_DISABLE_OPTIMIZATION
TArray<FStamp> AInfluenceMap::mStamps;

AInfluenceMap::AInfluenceMap()
{
	PrimaryActorTick.bCanEverTick = true;
}

uint32 AInfluenceMap::GetGridIndexFromCoords(FVector2D coords) const
{
	CheckCoordsValid(coords);
	return coords.X + coords.Y * GridRows;
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

bool AInfluenceMap::GetCoordsValid(FVector2D coords) const
{
	return !(coords.X < 0 || coords.X >= GridRows || coords.Y < 0 || coords.Y >= GridColumns);
}

void AInfluenceMap::CheckCoordsValid(FVector2D coords) const
{
	check(GetCoordsValid(coords));
}

float AInfluenceMap::GetValueAtIndex(uint32 index) const
{
	return mValues[index];
}

float AInfluenceMap::GetValueAtCoords(FVector2D coords) const
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

void AInfluenceMap::CheckHighestCell(uint32 i, float& highestValue)
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
	float highestValue = -2e32;

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] += map.mValues[i] * scalar;

		if (updateHighestPoint)
		{
			CheckHighestCell(i, highestValue);
		}
	}
}

void AInfluenceMap::MultiplyMap(const AInfluenceMap& map, float scalar, bool updateHighestPoint)
{
	CheckMapsCompatible(*this, map);
	float highestValue = -2e32;

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
	float highestValue = -2e32;

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
	float highestValue = -2e32;

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] = 1 - mValues[i];

		if (updateHighestPoint)
		{
			CheckHighestCell(i, highestValue);
		}
	}
}

int32 AInfluenceMap::GenerateStamp(EStampFunc funcType, uint32 radius)
{
	FStamp stamp{ funcType, radius, TArray<float>() };
	stamp.mValues.SetNumZeroed(radius * radius);
	const uint32 diameter = 2 * radius;

	switch (funcType)
	{
	case EStampFunc::LINEAR:
	{
		const FVector2D center = FVector2D(radius, radius);
		for (uint32 i = 0; i < diameter; i++)
		{
			for (uint32 j = 0; j < diameter; j++)
			{
				const float linearDistToCenter = (FVector2D(i, j) - center).Size();
				stamp.mValues[i + j * diameter] = linearDistToCenter;
			}
		}
		break;
	}
	default:
	{
		return -1;
	}
	}

	return mStamps.Add(stamp);
}

void AInfluenceMap::ApplyStamp(int32 stampIndex, FVector2D centerCoords)
{
	FStamp stamp = mStamps[stampIndex];
	const uint32 diameter = 2 * stamp.mRadius;

	switch (stamp.mFuncType)
	{
	case EStampFunc::LINEAR:
	{
		int32 stampValueIndex = 0;
		for (int32 i = centerCoords.X - stamp.mRadius; i < centerCoords.X + stamp.mRadius; i++)
		{
			for (int32 j = centerCoords.Y - stamp.mRadius; j < centerCoords.Y + stamp.mRadius; j++)
			{
				if (GetCoordsValid(FVector2D(i, j)))
				{
					mValues[i + j * diameter] = stamp.mValues[stampValueIndex++];
				}
			}
		}
		break;
	}
	default:
	{
		break;
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
			float cellValue = mValues[cellIndex];

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
PRAGMA_ENABLE_OPTIMIZATION
