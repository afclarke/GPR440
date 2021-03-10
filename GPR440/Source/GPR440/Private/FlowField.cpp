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
	mFlowField.SetNum(GridRows * GridColumns, false);
	mIntegrationField.SetNum(GridRows * GridColumns, false);

	BuildFlowField();
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
			const uint32 cellIndex = i + j * GridColumns;

			const FVector cellCenter = FVector(
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
			if (hit)
			{
				AActor* hitActor = costBoxTraceHitResult.GetActor();
				// blocker
				if (hitActor->ActorHasTag("CostFieldBlocker"))
				{
					cost = 255;
				}
				// goal
				else if (hitActor->ActorHasTag("CostFieldGoal"))
				{
					cost = 0;
					mGoalCell = FVector2D(i, j);
				}
				else
				{
					// cost is inverse distance of trace result (1-254)
					cost = 253 * (CostTraceLength - costBoxTraceHitResult.Distance) / CostTraceLength + 1;
				}
			}

			mCostField[cellIndex] = cost;
		}
	}
}

PRAGMA_DISABLE_OPTIMIZATION
void AFlowField::BuildIntegrationField()
{
	BuildCostField();

	for (uint32 i = 0; i < GridRows * GridColumns; i++)
	{
		mIntegrationField[i] = 65535;
	}
	const uint32 goalCellIndex = mGoalCell.X + mGoalCell.Y * GridColumns;
	mIntegrationField[goalCellIndex] = 0;

	TCircularBuffer<FVector2D> openCircularQueue(GridRows * GridColumns,
		FVector2D(-1, -1));
	uint32 queueBack = 0, queueHead = 0, queueSize = 0;

	// enqueue goal cell
	openCircularQueue[queueBack] = mGoalCell;
	queueBack = openCircularQueue.GetPreviousIndex(queueBack);
	queueSize++;

	while (queueSize > 0)
	{
		// dequeue
		FVector2D curCell = openCircularQueue[queueHead];
		openCircularQueue[queueHead] = FVector2D(-1, -1);
		queueHead = openCircularQueue.GetPreviousIndex(queueHead);
		queueSize--;

		const uint32 curCellIndex = curCell.X + curCell.Y * GridColumns;
		if(mCostField[curCellIndex] == 255)
		{
			mIntegrationField[curCellIndex] = 65535;
			continue;
		}

		TArray<FVector2D> neighbors = GetCellNeighbors(curCell);
		for (FVector2D neighborCell : neighbors)
		{
			if (neighborCell.X < 0) continue;

			const uint32 neighborCellIndex = neighborCell.X + neighborCell.Y * GridColumns;

			const uint16 endNodeCost = FMath::Min(mIntegrationField[curCellIndex] + mCostField[neighborCellIndex], 65535);
			if (endNodeCost < mIntegrationField[neighborCellIndex])
			{
				bool queueContainsNeighbor = false;
				for (uint32 i = 0; i < openCircularQueue.Capacity(); i++)
				{
					if(openCircularQueue[i] == neighborCell)
					{
						queueContainsNeighbor = true;
						break;
					}
				}
				if (!queueContainsNeighbor)
				{
					openCircularQueue[queueBack] = neighborCell;
					queueBack = openCircularQueue.GetPreviousIndex(queueBack);
					queueSize++;
				}
				
				mIntegrationField[neighborCellIndex] = endNodeCost;
			}
		}
	}
}

void AFlowField::BuildFlowField()
{
	BuildIntegrationField();

	for (uint32 i = 0; i < GridRows; i++)
	{
		for (uint32 j = 0; j < GridColumns; j++)
		{
			uint16 lowestCost = 65535;
			uint32 lowestCostCellIndex = -1;
			TArray<FVector2D> neighborCells = GetCellNeighbors(FVector2D(i, j));
			for (uint32 k = 0; k < 8; k++)
			{
				if (neighborCells[k].X < 0) continue;

				const uint32 neighborCellIndex = neighborCells[k].X + neighborCells[k].Y * GridColumns;
				if (mIntegrationField[neighborCellIndex] <= lowestCost)
				{
					lowestCost = mIntegrationField[neighborCellIndex];
					lowestCostCellIndex = k;
				}
			}
			mFlowField[i + j * GridColumns] = lowestCostCellIndex;
		}
	}
}

TArray<FVector2D> AFlowField::GetCellNeighbors(FVector2D cell)
{
	TArray<FVector2D> neighbors;
	neighbors.SetNumUninitialized(8);

	FVector2D topLeft = FVector2D(cell.X - 1, cell.Y - 1);
	neighbors[0] = topLeft.X >= 0 && topLeft.X < GridRows
		&& topLeft.Y >= 0 && topLeft.Y < GridColumns ?
		topLeft : FVector2D(-1, -1);

	FVector2D topMid = FVector2D(cell.X - 0, cell.Y - 1);
	neighbors[1] = topMid.X >= 0 && topMid.X < GridRows
		&& topMid.Y >= 0 && topMid.Y < GridColumns ?
		topMid : FVector2D(-1, -1);

	FVector2D topRight = FVector2D(cell.X + 1, cell.Y - 1);
	neighbors[2] = topRight.X >= 0 && topRight.X < GridRows
		&& topRight.Y >= 0 && topRight.Y < GridColumns ?
		topRight : FVector2D(-1, -1);

	FVector2D left = FVector2D(cell.X - 1, cell.Y + 0);
	neighbors[3] = left.X >= 0 && left.X < GridRows
		&& left.Y >= 0 && left.Y < GridColumns ?
		left : FVector2D(-1, -1);

	FVector2D right = FVector2D(cell.X + 1, cell.Y + 0);
	neighbors[4] = right.X >= 0 && right.X < GridRows
		&& right.Y >= 0 && right.Y < GridColumns ?
		right : FVector2D(-1, -1);

	FVector2D botLeft = FVector2D(cell.X - 1, cell.Y + 1);
	neighbors[5] = botLeft.X >= 0 && botLeft.X < GridRows
		&& botLeft.Y >= 0 && botLeft.Y < GridColumns ?
		botLeft : FVector2D(-1, -1);

	FVector2D botMid = FVector2D(cell.X - 0, cell.Y + 1);
	neighbors[6] = botMid.X >= 0 && botMid.X < GridRows
		&& botMid.Y >= 0 && botMid.Y < GridColumns ?
		botMid : FVector2D(-1, -1);

	FVector2D botRight = FVector2D(cell.X + 1, cell.Y + 1);
	neighbors[7] = botRight.X >= 0 && botRight.X < GridRows
		&& botRight.Y >= 0 && botRight.Y < GridColumns ?
		botRight : FVector2D(-1, -1);

	return neighbors;
}

void AFlowField::DrawDebug() const
{
	UWorld* pWorld = GetWorld();
	for (uint32 i = 0; i < GridRows; i++)
	{
		for (uint32 j = 0; j < GridColumns; j++)
		{
			const uint32 cellIndex = i + j * GridColumns;

			FVector cellCenter = FVector(
				mGridOrigin.X + mCellHalfDims.X + mCellDims.X * i,
				mGridOrigin.Y + mCellHalfDims.Y + mCellDims.Y * j,
				mGridOrigin.Z);

			if (DrawGrid)
			{
				// draw grid slightly higher than others
				FVector gridCellLoc = cellCenter;
				gridCellLoc.Z += 1.0f;
				DrawDebugBox(pWorld, gridCellLoc, mCellHalfDims, FColor::Yellow,
					false, -1, 0, 20);
			}
			if (DrawCost)
			{
				const float cost = mCostField[cellIndex];
				DrawDebugSolidBox(pWorld, FBox(cellCenter - mCellHalfDims, cellCenter + mCellHalfDims),
					FColor(cost, 255.0f - cost, 0, 255));
			}
			if(DrawIntegration)
			{
				float integrationCost = mIntegrationField[cellIndex];
				//integrationCost /= 65535;
				//integrationCost *= 255;
				DrawDebugSolidBox(pWorld, FBox(cellCenter - mCellHalfDims, cellCenter + mCellHalfDims),
					FColor(integrationCost, 255.0f - integrationCost, 0, 255));
			}
			if (DrawFlow)
			{
				DrawDebugLine(pWorld, cellCenter, cellCenter + GetFlow(cellIndex) * mCellHalfDims.X,
					FColor::Cyan, false, -1, 0, 15);
			}
		}
	}
}
PRAGMA_ENABLE_OPTIMIZATION
