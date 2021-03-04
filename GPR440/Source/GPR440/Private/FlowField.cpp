// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowField.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

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

	mCostField.SetNum(GridRows * GridColumns, false);
	
	BuildCostField();
}

void AFlowField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(DrawDebug)
	{
		DrawGrid();
	}
}

uint32 AFlowField::GetGridIndex(FVector loc)
{
	uint32 x = FMath::FloorToInt((loc.X - mGridOrigin.X) / mCellDims.X);
	uint32 y = FMath::FloorToInt((loc.Y - mGridOrigin.Y) / mCellDims.Y);
	return x + y * GridColumns;
}

void AFlowField::BuildCostField()
{
	UWorld* pWorld = GetWorld();
	for (uint32 i = 0; i < GridRows; i++)
	{
		for (uint32 j = 0; j < GridColumns; j++)
		{
			FVector cellCenter = FVector(
				mGridOrigin.X + mCellHalfDims.X + mCellDims.X * i,
				mGridOrigin.Y + mCellHalfDims.Y + mCellDims.Y * j,
				mGridOrigin.Z);
			FVector cellTraceEnd = cellCenter;
			cellTraceEnd.Z = 150;

			FHitResult costBoxTraceHitResult;
			bool hit = UKismetSystemLibrary::BoxTraceSingle(pWorld, cellCenter, cellTraceEnd, mCellHalfDims,
				FRotator::ZeroRotator, CostTraceChannel, false, TArray<AActor*>(),
				EDrawDebugTrace::Type::ForDuration, costBoxTraceHitResult, true);

			mCostField[i + j * GridColumns] = hit;
		}
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
			
			DrawDebugBox(pWorld, cellCenter, mCellHalfDims, mCostField[i + j * GridColumns] ? FColor::Red : FColor::Blue,
				false, -1, 0, 20);
		}
	}
}

