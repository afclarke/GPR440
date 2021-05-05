// Fill out your copyright notice in the Description page of Project Settings.


#include "TankFlowField.h"
#include "Powerhead.h"
#include "DrawDebugHelpers.h"

PRAGMA_DISABLE_OPTIMIZATION
ATankFlowField::ATankFlowField()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATankFlowField::DebugDraw() const
{
	if (!mDrawDebug) return;
	
	float locZ = GetActorLocation().Z;
	// draw row lines
	for(int32 i = 0; i < mNumRows; i++)
	{
		FVector2D beginLine = mRectBounds.Min;
		FVector2D endLine = FVector2D(mRectBounds.Min.X, mRectBounds.Max.Y);
		beginLine.X += i * mCellDims.X;
		endLine.X += i * mCellDims.X;
		DrawDebugLine(GetWorld(), FVector(beginLine, locZ),
			FVector(endLine, locZ), FColor::Cyan);
	}
	// draw column lines
	for (int32 i = 0; i < mNumColumns; i++)
	{
		FVector2D beginLine = mRectBounds.Min;
		FVector2D endLine = FVector2D(mRectBounds.Max.X, mRectBounds.Min.Y);
		beginLine.Y += i * mCellDims.Y;
		endLine.Y += i * mCellDims.Y;
		DrawDebugLine(GetWorld(), FVector(beginLine, locZ),
			FVector(endLine, locZ), FColor::Cyan);
	}
	// draw flow arrows
	for(int32 i = 0; i < mFlow.Num(); i++)
	{
		FVector loc = FVector(GetWorldLocAtIndex(i), locZ);
		loc += FVector(mCellDims / 2.0f, 0);
		DrawDebugDirectionalArrow(GetWorld(), loc,
			loc + FVector(
				mFlow[i].GetSafeNormal() * mFlow[i].Size() * mDebugArrowLengthScalar, 0),
			400, FColor::Blue, false, -1, 0,
			mDebugArrowThickness);
	}
}

void ATankFlowField::AddPowerhead(APowerhead* pPowerhead)
{
	if(!mInitted)
	{
		Init();
	}
	mPowerheads.Add(pPowerhead);
	CalcFlow();
}

FVector2D ATankFlowField::GetWorldLocAtIndex(int32 index) const
{
	FVector2D loc = mRectBounds.Min;
	FVector2D gridLoc = FVector2D(FMath::FloorToInt(index / mNumColumns), index % mNumColumns);
	loc += FVector2D(gridLoc.X * mCellDims.X, gridLoc.Y * mCellDims.Y);
	return loc;
}

FVector2D ATankFlowField::GetFlowAtLoc(FVector loc)
{
	int32 index = GetIndexAtLoc(loc);
	return mFlow[index];
}

FVector2D ATankFlowField::GetGridLocAtLoc(FVector loc) const
{
	FVector2D gridLoc;
	FVector2D loc2D = FVector2D(loc);
	loc2D -= mRectBounds.Min;
	gridLoc.X = FMath::FloorToInt(loc2D.X / mCellDims.X);
	gridLoc.Y = FMath::FloorToInt(loc2D.Y / mCellDims.Y);
	return gridLoc;
}

int32 ATankFlowField::GetIndexAtLoc(FVector loc) const
{
	FVector2D gridLoc = GetGridLocAtLoc(loc);
	return gridLoc.X * mNumColumns + gridLoc.Y;
}

void ATankFlowField::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

void ATankFlowField::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	mPowerheads.Empty();
}

void ATankFlowField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalcFlow();
}

void ATankFlowField::Init()
{
	mFlow.SetNumUninitialized(mNumRows * mNumColumns);
	FVector2D dims = mRectBounds.Max - mRectBounds.Min;
	mCellDims = FVector2D(dims.X / mNumRows, dims.Y / mNumColumns);
	mInitted = true;
}

void ATankFlowField::CalcFlow()
{
	for(int32 i = 0; i < mFlow.Num(); i++)
	{
		// reset flow
		mFlow[i] = FVector2D::ZeroVector;

		// add flow of all powerheads
		FVector2D loc = GetWorldLocAtIndex(i);
		for (APowerhead* powerhead : mPowerheads)
		{
			FVector2D cellToPowerhead = loc - FVector2D(powerhead->GetActorLocation());
			cellToPowerhead /= mCellDims;
			float cellToPowerheadDist = cellToPowerhead.Size();
			FVector2D powerheadFlowAtCell = powerhead->mFlow / cellToPowerheadDist;
			mFlow[i] += powerheadFlowAtCell;
		}
	}
}
PRAGMA_ENABLE_OPTIMIZATION
