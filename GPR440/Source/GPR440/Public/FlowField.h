// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlowField.generated.h"

UCLASS()
class GPR440_API AFlowField : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlowField();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	uint32 GetGridIndex(FVector loc);
	void BuildCostField();

private:
	void DrawDebug() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DrawGrid = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DrawCost = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DrawFlow = false;
	
protected:
	UPROPERTY(EditAnywhere)
	float GridWidth;
	UPROPERTY(EditAnywhere)
	float GridHeight;
	UPROPERTY(EditAnywhere)
	uint32 GridRows;
	UPROPERTY(EditAnywhere)
	uint32 GridColumns;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETraceTypeQuery> CostTraceChannel;
	UPROPERTY(EditAnywhere)
	float CostTraceLength;
	
	FVector mGridOrigin;
	FVector mCellDims;
	FVector mCellHalfDims;
	TArray<uint8> mFlowField;
	TArray<uint8> mCostField;
	
	const TMap<uint8, FVector> FLOW_DIR
	{
		{0, FVector(1, 0, 0)},
		{1, FVector(1, 1, 0)},
		{2, FVector(0, 1, 0)},
		{3, FVector(-1, 1, 0)},
		{4, FVector(-1, 0, 0)},
		{5, FVector(-1, -1, 0)},
		{6, FVector(0, -1, 0)},
		{7, FVector(1, -1, 0)}
	};

};
