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

	DrawDebug();
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
			cellTraceEnd.Z -= CostTraceLength;

			FHitResult costBoxTraceHitResult;
			bool hit = UKismetSystemLibrary::BoxTraceSingle(pWorld, cellCenter, cellTraceEnd, mCellHalfDims,
				FRotator::ZeroRotator, CostTraceChannel, false, TArray<AActor*>(),
				EDrawDebugTrace::Type::None, costBoxTraceHitResult, true);

			uint8 cost = 1;
			if(hit)
			{
				AActor* hitActor = costBoxTraceHitResult.GetActor();
				// blocker
				if (hitActor->ActorHasTag("CostFieldBlocker"))
				{
					cost = 255;
				}
				// goal
				else if(hitActor->ActorHasTag("CostFieldGoal"))
				{
					cost = 0;
				}
				else
				{
					// cost is inverse distance of trace result (1-254)
					cost = 253 * (CostTraceLength - costBoxTraceHitResult.Distance) / CostTraceLength + 1;
				}
			}
			
			mCostField[i + j * GridColumns] = cost;
		}
	}
}

void AFlowField::DrawDebug() const
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

			if(DrawGrid)
			{
				// draw grid slightly higher than others
				FVector gridCellLoc = cellCenter;
				gridCellLoc.Z += 1.0f;
				DrawDebugBox(pWorld, gridCellLoc, mCellHalfDims, FColor::Yellow,
					false, -1, 0, 20);
			}
			if(DrawCost)
			{
				const float cost = mCostField[i + j * GridColumns];
				DrawDebugSolidBox(pWorld, FBox(cellCenter - mCellHalfDims, cellCenter + mCellHalfDims),
					FColor(cost, 255.0f - cost, 0, 255));
			}
			if(DrawFlow)
			{
				
			}
		}
	}
}
