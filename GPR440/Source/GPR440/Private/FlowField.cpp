// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowField.h"
#include "DrawDebugHelpers.h"

AFlowField::AFlowField()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlowField::BeginPlay()
{
	Super::BeginPlay();

	mGridOrigin = GetActorLocation();
	mCellDims = FVector(GridWidth / GridRows, GridHeight / GridColumns, 0);
	mCellHalfDims = mCellDims / 2.0f;
}

void AFlowField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(DrawDebug)
	{
		DrawGrid();
	}
}

void AFlowField::DrawGrid() const
{
	UWorld* pWorld = GetWorld();
	for(uint32 i = 0; i < GridRows; i++)
	{
		for(uint32 j = 0; j < GridColumns; j++)
		{
			FVector cellCenter = FVector(
				mGridOrigin.X + mCellHalfDims.X + mCellDims.X * i,
				mGridOrigin.Y + mCellHalfDims.Y + mCellDims.Y * j,
				mGridOrigin.Z);
			
			DrawDebugBox(pWorld, cellCenter, mCellHalfDims, FColor::Blue,
				false, -1, 0, 20);
		}
	}
}

