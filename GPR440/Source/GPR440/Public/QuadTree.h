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
	UQuadTree();
	~UQuadTree();

public:
	void Init(int32 depth, FBox2D rect);
	bool Insert(AActor* pActor);
	void Clear();
	TArray<AActor*> QuerySqrRadius(AActor* pActor, float sqrRadius);
	void Draw(UWorld* pWorld) const;

private:
	void Subdivide();
	
private:
	const int32 mCapacity = 10;
	const int32 mMaxDepth = 5;

	int32 mDepth;
	FBox2D mRect;
	TArray<AActor*> mpActors;
	UQuadTree* mpNodes[4];
	bool mSubdivided = false;
	bool mIsLeaf = false;
};
