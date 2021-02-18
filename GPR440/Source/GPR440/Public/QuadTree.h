// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class GPR440_API QuadTree
{
public:
	QuadTree(int32 depth, FBox2D rect);
	~QuadTree();

public:
	bool Insert(AActor* pActor);
	void Clear();
	TArray<AActor*> QuerySqrRadius(AActor* pActor, float sqrRadius);
	void Draw(UWorld* pWorld) const;

private:
	void Subdivide();
	
private:
	const int32 mCapacity = 3;
	const int32 mMaxDepth = 4;

	int32 mDepth;
	FBox2D mRect;
	TArray<AActor*> mpActors;
	QuadTree* mpNodes[4];
	bool mSubdivided = false;
	bool mIsLeaf = false;
};
