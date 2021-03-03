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

private:
	void DrawGrid() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DrawDebug = true;
	
protected:
	UPROPERTY(EditAnywhere)
	float GridWidth;
	UPROPERTY(EditAnywhere)
	float GridHeight;
	UPROPERTY(EditAnywhere)
	uint32 GridRows;
	UPROPERTY(EditAnywhere)
	uint32 GridColumns;
	
	FVector mGridOrigin;
	FVector mCellDims;
	FVector mCellHalfDims;
	TArray<uint8> mFlowField;

};
