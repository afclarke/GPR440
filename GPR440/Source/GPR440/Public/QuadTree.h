// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuadTree.generated.h"

/**
 * 
 */
UCLASS()
class GPR440_API UQuadTree : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Init(int32 depth, FBox2D rect);
	UFUNCTION(BlueprintCallable)
	bool Insert(AActor* pActor);
	UFUNCTION(BlueprintCallable)
	void Clear();
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> QuerySqrRadius(AActor* pActor, float sqrRadius, FName forTag = NAME_None);
	UFUNCTION(BlueprintCallable)
	void Draw(UWorld* pWorld) const;

protected:
	UPROPERTY(EditDefaultsOnly)
	int32 mCapacity = 3;
	UPROPERTY(EditDefaultsOnly)
	int32 mMaxDepth = 5;
	
private:
	void Subdivide();
	bool PointIsInsideOrOn(const FVector2D& point) const;
	
	int32 mDepth;
	FBox2D mRect;
	bool mSubdivided = false;
	bool mIsLeaf = false;
	UPROPERTY()
	TArray<AActor*> mpActors;
	UPROPERTY()
	UQuadTree* mpNodes[4];
};
