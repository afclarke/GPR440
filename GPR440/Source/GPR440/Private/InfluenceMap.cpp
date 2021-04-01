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

void AInfluenceMap::Construct(AInfluenceMap* const parentMap, FVector2D gridOriginCoords, uint32 gridRows, uint32 gridColumns)
{
	mpParentMap = parentMap;
	mpInfluenceMapManager = mpParentMap->mpInfluenceMapManager;
	mCellDims = mpParentMap->mCellDims;
	mCellHalfDims = mpParentMap->mCellHalfDims;
	GridRows = gridRows;
	GridColumns = gridColumns;
	GridWidth = mCellDims.X * GridRows;
	GridHeight = mCellDims.Y * GridColumns;

	SetOriginCoords(gridOriginCoords);
	
	mValues.SetNumZeroed(GridRows * GridColumns);
	SetActorLocation(mGridOriginWorld);
	mpInfluenceMapManager->RegisterMap(this);
}

uint32 AInfluenceMap::GetGridIndexFromCoords(FVector2D coords) const
{
	CheckCoordsValid(coords);
	return coords.X + coords.Y * GridRows;
}

FVector2D AInfluenceMap::GetGridCoordsFromWorldLoc(FVector loc) const
{
	FVector gridSpaceLoc = loc - mGridOriginWorld;
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
	return mGridOriginWorld + FVector(worldLoc, 0);
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

void AInfluenceMap::SetCenterCoords(FVector2D centerCoords)
{
	const FVector2D originCoords = centerCoords - FVector2D(GridRows / 2, GridColumns / 2);
	SetOriginCoords(originCoords);
}

void AInfluenceMap::SetOriginCoords(FVector2D originCoords)
{
	mGridOriginCoords = originCoords;
	mGridOriginWorld = mpParentMap->mGridOriginWorld + FVector(mGridOriginCoords, 0) * mCellDims;
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

void AInfluenceMap::CheckMapsCompatible(const AInfluenceMap& mapA, const AInfluenceMap& mapB)
{
	check(mapA.GridRows == mapB.GridRows && mapA.GridColumns == mapB.GridColumns);
}

int32 AInfluenceMap::CalcHighestCellIndex()
{
	int32 highestCellIndex = -1;
	float highestCellValue = -1e32;

	// start search from random cell in grid
	const int32 beginSearchIndex = FMath::RandRange(0, mValues.Num() - 2);
	int32 i = beginSearchIndex + 1;
	while(i != beginSearchIndex)
	{
		if (mValues[i] > highestCellValue)
		{
			highestCellValue = mValues[i];
			highestCellIndex = i;
		}

		// loop i;
		if(++i >= mValues.Num())
		{
			i = 0;
		}
	}
	return highestCellIndex;
}

TArray<int32> AInfluenceMap::CalcHighestCellsIndicies(int32 numHighestCells)
{
	TArray<int32> highestCellsIndices;
	TArray<float> highestCellsValues;
	for(int32 i = 0; i < numHighestCells; i++)
	{
		highestCellsIndices.Add(-1);
		highestCellsValues.Add(-1e32);
	}

	// start search from random cell in grid
	const int32 beginSearchIndex = FMath::RandRange(0, mValues.Num() - 2);
	int32 i = beginSearchIndex + 1;
	while (i != beginSearchIndex)
	{
		for(int32 j = 0; j < numHighestCells; j++)
		{
			if (mValues[i] > highestCellsValues[j])
			{
				highestCellsValues[j] = mValues[i];
				highestCellsIndices[j] = i;
				break;
			}
		}


		// loop i;
		if (++i >= mValues.Num())
		{
			i = 0;
		}
	}
	return highestCellsIndices;
}

void AInfluenceMap::LiftFromParentMap(const AInfluenceMap& map, float scalar)
{
	for(uint32 x = 0; x < GridRows; x++)
	{
		for(uint32 y = 0; y < GridColumns; y++)
		{
			FVector2D toCoords = FVector2D(x, y);
			int32 toIndex = GetGridIndexFromCoords(toCoords);
			FVector2D fromCoords = mGridOriginCoords + toCoords;
			if(map.GetCoordsValid(fromCoords))
			{
				mValues[toIndex] += map.GetValueAtCoords(fromCoords) * scalar;
			}
		}
	}
}

void AInfluenceMap::MultiplyMap(const AInfluenceMap& map, float scalar)
{
	CheckMapsCompatible(*this, map);

	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] *= map.mValues[i] * scalar;
	}
}

void AInfluenceMap::ScaleMap(float scalar)
{
	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] *= scalar;
	}
}

void AInfluenceMap::InvertMap(bool updateHighestPoint)
{
	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] = 1 - mValues[i];
	}
}

void AInfluenceMap::ClearMap()
{
	for (int32 i = 0; i < mValues.Num(); i++)
	{
		mValues[i] = 0;
	}
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

void AInfluenceMap::ApplyStamp(int32 stampIndex, FVector2D centerCoords, float scalar, bool multiply)
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
				if(multiply)
				{
					mValues[GetGridIndexFromCoords(coords)] *= stamp.mValues[i] * scalar;
				}
				else
				{
					mValues[GetGridIndexFromCoords(coords)] += stamp.mValues[i] * scalar;
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

	if(!mpParentMap)
	{
		mGridOriginWorld = GetActorLocation();
		mCellDims = FVector(GridWidth / GridRows, GridHeight / GridColumns, 0);
		mCellHalfDims = mCellDims / 2.0f;
		mValues.SetNumZeroed(GridRows * GridColumns);

		mpInfluenceMapManager = Cast<AInfluenceMapManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), AInfluenceMapManager::StaticClass()));
		mpInfluenceMapManager->RegisterMap(this);
	}
}

void AInfluenceMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawGrid();
}
PRAGMA_ENABLE_OPTIMIZATION
