// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InfluenceMap.generated.h"

UCLASS(Abstract)
class GPR440_API AInfluenceMap : public AActor
{
	GENERATED_BODY()
	
public:	
	AInfluenceMap();

	uint32 GetGridIndexFromCoords(FVector2D coords) const;
	FVector2D GetGridCoordsFromWorldLoc(FVector loc) const;
	uint32 GetGridIndexFromWorldLoc(FVector loc) const;

	FVector GetWorldLocFromCoords(FVector2D coords) const;

private:
	void DrawGrid() const;

public:
	UPROPERTY(EditAnywhere)
	float GridWidth;
	UPROPERTY(EditAnywhere)
	float GridHeight;
	UPROPERTY(EditAnywhere)
	uint32 GridRows;
	UPROPERTY(EditAnywhere)
	uint32 GridColumns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DebugDrawGrid = false;
	
	UPROPERTY(VisibleAnywhere)
	FVector mGridOrigin;
	UPROPERTY(VisibleAnywhere)
	FVector mCellDims;
	UPROPERTY(VisibleAnywhere)
	FVector mCellHalfDims;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
