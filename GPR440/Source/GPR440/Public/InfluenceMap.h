// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InfluenceMap.generated.h"

UENUM()
enum class EStampFunc : uint8
{
	LINEAR,
	INVERSE_QUADRATIC
};

USTRUCT()
struct FStamp
{
	GENERATED_BODY()
	
	EStampFunc mFuncType;
	uint32 mRadius;
	TArray<float> mValues;
};

UCLASS()
class GPR440_API AInfluenceMap : public AActor
{
	GENERATED_BODY()
	
public:	
	AInfluenceMap();

	void Construct(AInfluenceMap* const parentMap, FVector2D gridOriginCoords, uint32 gridRows, uint32 gridColumns);
	
	uint32 GetGridIndexFromCoords(FVector2D coords) const;
	FVector2D GetGridCoordsFromWorldLoc(FVector loc) const;
	uint32 GetGridIndexFromWorldLoc(FVector loc) const;
	FVector GetWorldLocFromCoords(FVector2D coords) const;
	FVector2D GetGridCoordsFromIndex(uint32 index) const;
	FVector GetWorldLocFromIndex(uint32 index) const;
	bool GetCoordsValid(FVector2D coords) const;
	void CheckCoordsValid(FVector2D coords) const;
	void CheckIndexValid(uint32 index) const;

	void SetCenterCoords(FVector2D centerCoords);
	void SetOriginCoords(FVector2D originCoords);

	float GetValueAtIndex(uint32 index) const;
	float GetValueAtCoords(FVector2D coords) const;

	static void CheckMapsCompatible(const AInfluenceMap& mapA, const AInfluenceMap& mapB);
	int32 CalcHighestCellIndex();
	void AddMap(const AInfluenceMap& map, float scalar = 1.0f);
	void MultiplyMap(const AInfluenceMap& map, float scalar = 1.0f);
	void ScaleMap(float scalar);
	void InvertMap(bool updateHighestPoint = false);
	void ClearMap();

	static int32 GenerateStamp(EStampFunc funcType, uint32 radius);
	void ApplyStamp(int32 stampIndex, FVector2D centerCoords, float scalar = 1, bool multiply = false);

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
	bool DebugDrawValue = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor DebugValueColorPos = FColor::Green;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor DebugValueColorNeg = FColor::Magenta;
	
	UPROPERTY(VisibleAnywhere)
	FVector mGridOriginWorld;
	UPROPERTY(VisibleAnywhere)
	FVector2D mGridOriginCoords;
	UPROPERTY(VisibleAnywhere)
	FVector mCellDims;
	UPROPERTY(VisibleAnywhere)
	FVector mCellHalfDims;

private:
	AInfluenceMap* mpParentMap;
	class AInfluenceMapManager* mpInfluenceMapManager;
	
	TArray<float> mValues;

	static TArray<FStamp> mStamps;
};
