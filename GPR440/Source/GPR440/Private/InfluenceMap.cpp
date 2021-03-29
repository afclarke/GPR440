// Fill out your copyright notice in the Description page of Project Settings.


#include "InfluenceMap.h"
#include "DrawDebugHelpers.h"
#include "InfluenceMapManager.h"
#include "Kismet/GameplayStatics.h"

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
	FVector2D gridCoords = FVector2D(gridSpaceLoc.X / GridWidth * GridRows, gridSpaceLoc.Y / GridHeight * GridColumns);
	gridCoords.X = FMath::FloorToInt(gridCoords.X);
	gridCoords.Y = FMath::FloorToInt(gridCoords.Y);
	CheckCoordsValid(gridCoords);
	UE_LOG(LogTemp, Warning, TEXT("GRID COORDS (%f, %f)"), gridCoords.X, gridCoords.Y)
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
	FVector2D worldLoc = coords / FVector2D(GridRows, GridColumns) * FVector2D(GridWidth, GridHeight);
	return mGridOrigin + FVector(worldLoc, 0);
}

FVector2D AInfluenceMap::GetGridCoordsFromIndex(uint32 index) const
{
	CheckIndexValid(index);
	return FVector2D(index % GridRows, index / GridRows);
}

FVector AInfluenceMap::GetWorldLocFromIndex(uint32 index) const
{
	const FVector2D coords = GetGridCoordsFromIndex(index);
	return GetWorldLocFromCoords(coords);
}

bool AInfluenceMap::GetCoordsValid(FVector2D coords) const
{
	return !(coords.X < 0 || coords.X >= GridRows || coords.Y < 0 || coords.Y >= GridColumns);
}

void AInfluenceMap::CheckCoordsValid(FVector2D coords) const
{
	check(GetCoordsValid(coords));
}

void AInfluenceMap::CheckIndexValid(uint32 index) const
{
	check(index < GridRows* GridColumns);
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

void AInfluenceMap::CheckHighestCell(uint32 i)
{
	// check for highest value cell
	if (mValues[i] >= mHighestCellValue)
	{
		mHighestCellValue = mValues[i];
		mHighestCellIndex = i;
	}
}

void AInfluenceMap::AddMap(const AInfluenceMap& map, float scalar, bool updateHighestPoint)
{
	CheckMapsCompatible(*this, map);
	if (updateHighestPoint)
	{
		mHighestCellValue = -2e32;
	}

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] += map.mValues[i] * scalar;

		if (updateHighestPoint)
		{
			CheckHighestCell(i);
		}
	}
}

void AInfluenceMap::MultiplyMap(const AInfluenceMap& map, float scalar, bool updateHighestPoint)
{
	CheckMapsCompatible(*this, map);
	if (updateHighestPoint)
	{
		mHighestCellValue = -2e32;
	}

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] *= map.mValues[i] * scalar;

		if (updateHighestPoint)
		{
			CheckHighestCell(i);
		}
	}
}

void AInfluenceMap::ScaleMap(float scalar, bool updateHighestPoint)
{
	if (updateHighestPoint)
	{
		mHighestCellValue = -2e32;
	}

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] *= scalar;

		// TODO: This is only necessary if map values & scalars can be negative
		if (updateHighestPoint)
		{
			CheckHighestCell(i);
		}
	}
}

void AInfluenceMap::InvertMap(bool updateHighestPoint)
{
	if (updateHighestPoint)
	{
		mHighestCellValue = -2e32;
	}

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] = 1 - mValues[i];

		if (updateHighestPoint)
		{
			CheckHighestCell(i);
		}
	}
}

void AInfluenceMap::ClearMap()
{
	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] = 0;
	}
}

void AInfluenceMap::ConstuctFrom(const AInfluenceMap& other)
{
	mpInfluenceMapManager = other.mpInfluenceMapManager;
	GridWidth = other.GridWidth;
	GridHeight = other.GridHeight;
	GridRows = other.GridRows;
	GridColumns = other.GridColumns;
	mGridOrigin = other.mGridOrigin;
	mCellDims = other.mCellDims;
	mCellHalfDims = other.mCellHalfDims;
	mValues.SetNumZeroed(GridRows * GridColumns);
	SetActorLocation(mGridOrigin);
}

int32 AInfluenceMap::GenerateStamp(EStampFunc funcType, uint32 radius)
{
	FStamp stamp{ funcType, radius, TArray<float>() };
	const uint32 diameter = 2 * radius;
	stamp.mValues.SetNumZeroed(diameter * diameter);

	switch (funcType)
	{
	case EStampFunc::LINEAR:
	{
		const FVector2D center = FVector2D(radius, radius);
		for (uint32 i = 0; i < diameter; i++)
		{
			for (uint32 j = 0; j < diameter; j++)
			{
				float linearDistFromCenter = radius - (FVector2D(i, j) - center).Size();
				linearDistFromCenter = FMath::Max(linearDistFromCenter, 0.0f);
				linearDistFromCenter /= radius;
				stamp.mValues[i + j * diameter] = linearDistFromCenter;
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

void AInfluenceMap::ApplyStamp(int32 stampIndex, FVector2D centerCoords, float scalar)
{
	FStamp stamp = mStamps[stampIndex];
	const uint32 diameter = 2 * stamp.mRadius;

	switch (stamp.mFuncType)
	{
	case EStampFunc::LINEAR:
	{
		const FVector2D stampBottomLeft = centerCoords - FVector2D(stamp.mRadius, stamp.mRadius);
		for (int32 i = 0; i < stamp.mValues.Num(); i++)
		{
			const FVector2D coords = stampBottomLeft + FVector2D(i % diameter, FMath::FloorToInt(i / diameter));
			if (GetCoordsValid(coords))
			{
				mValues[GetGridIndexFromCoords(coords)] += stamp.mValues[i] * scalar;
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

			if (DebugDrawGrid)
			{
				DrawDebugBox(pWorld, cellCenter, mCellHalfDims, FColor::Yellow,
					false, -1, 0, 20);
			}
			if (DebugDrawValue)
			{
				cellCenter.Z += 1;
				FColor valueColor = cellValue > 0 ? DebugValueColorPos : DebugValueColorNeg;
				valueColor.A = FMath::Min(FMath::Abs(cellValue) * 255.0f, 255.0f);
				DrawDebugSolidBox(pWorld, FBox(cellCenter - mCellHalfDims, cellCenter + mCellHalfDims),
					valueColor);
			}
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

	mpInfluenceMapManager = Cast<AInfluenceMapManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AInfluenceMapManager::StaticClass()));
	mpInfluenceMapManager->RegisterMap(this);
}

void AInfluenceMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawGrid();
}
PRAGMA_ENABLE_OPTIMIZATION
