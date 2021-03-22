// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InfluenceMap.generated.h"

UENUM()
enum class EStampFunc : uint8
{
	LINEAR
};

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
	void CheckCoordsValid(FVector2D coords) const;

	uint8 GetValueAtIndex(uint32 index) const;
	uint8 GetValueAtCoords(FVector2D coords) const;
	uint32 GetHighestCellIndex() const;

	static void CheckMapsCompatible(const AInfluenceMap& mapA, const AInfluenceMap& mapB);
	void CheckHighestCell(uint32 i, uint8& highestValue);
	void AddMap(const AInfluenceMap& map, float scalar = 1.0f, bool updateHighestPoint = false);
	void MultiplyMap(const AInfluenceMap& map, float scalar = 1.0f, bool updateHighestPoint = false);
	void ScaleMap(float scalar, bool updateHighestPoint = false);
	void InvertMap(bool updateHighestPoint = false);

	static void GenerateStamp(EStampFunc funcType, uint32 radius);
	void PlaceStamp(EStampFunc funcType, uint32 radius, FVector2D centerCoords);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor DebugValueColor = FColor::Green;
	
	UPROPERTY(VisibleAnywhere)
	FVector mGridOrigin;
	UPROPERTY(VisibleAnywhere)
	FVector mCellDims;
	UPROPERTY(VisibleAnywhere)
	FVector mCellHalfDims;

private:
	TArray<uint8> mValues;
	uint32 mHighestCellIndex = -1;

	// maps from func type -> radius -> stamp values
	static TMap<EStampFunc, TMap<uint32, TArray<uint8>>> mStamps;
};
