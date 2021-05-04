// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TankFlowField.generated.h"

class APowerhead;

UCLASS()
class GPR440_API ATankFlowField : public AActor
{
	GENERATED_BODY()
	
public:	
	ATankFlowField();

	void CalcFlow();
	void DebugDraw() const;

	void AddPowerhead(APowerhead* pPowerhead);

	UFUNCTION(BlueprintCallable)
	FVector2D GetWorldLocAtIndex(int32 index) const;
	UFUNCTION(BlueprintCallable)
	FVector2D GetFlowAtLoc(FVector loc);
	UFUNCTION(BlueprintCallable)
	FVector2D GetGridLocAtLoc(FVector loc) const;
	UFUNCTION(BlueprintCallable)
	int32 GetIndexAtLoc(FVector loc) const;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

private:
	void Init();
	
public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FBox2D mRectBounds;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int32 mNumRows;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int32 mNumColumns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool mDrawDebug = true;
	UPROPERTY(EditAnywhere)
	float mDebugArrowLengthScalar = 0.5f;
	UPROPERTY(EditAnywhere)
	float mDebugArrowThickness = 10.f;

private:
	FVector2D mCellDims;
	bool mInitted = false;

	UPROPERTY()
	TArray<APowerhead*>  mPowerheads;
	UPROPERTY()
	TArray<FVector2D> mFlow;
	
};
